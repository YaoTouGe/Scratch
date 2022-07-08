#include <memory>
#include <fstream>
#include <string>
#include <unordered_set>
#include "ShaderUtil.h"
#include "RenderManager.h"
#include "Constants.h"

using namespace std;

namespace RenderSystem
{
    static std::string versionString = "#version 460 core";

    size_t ReadFileContent(const std::string &file, char *writeBuffer, size_t bufferSize)
    {
        ifstream ifs(file);

        if (!ifs.good())
        {
            ifs.close();
            return 0;
        }

        if (writeBuffer == nullptr)
        {
            auto startPos = ifs.tellg();
            ifs.seekg(0, ios::end);
            auto endPos = ifs.tellg();
            ifs.close();
            return endPos - startPos;
        }
        
        ifs.read(writeBuffer, bufferSize);
        ifs.close();
        return bufferSize;
    }


    void GetParentDirectoryFromFilePath(const std::string &path, std::string &outDir)
    {
        auto idx = path.find_last_of('/');
        if (idx != std::string::npos)
        {
            outDir = path.substr(0, idx + 1);
        }
    }

    ShaderProgram::SP ShaderUtil::LoadProgramFromRaw(const std::string &vertFile, const std::string &fragFile)
    {
        size_t fileSize = ReadFileContent(vertFile, nullptr, 0);
        char *vertFileBuffer = new char[fileSize + 1];
        ReadFileContent(vertFile, vertFileBuffer, fileSize);
        vertFileBuffer[fileSize] = '\0';

        fileSize = ReadFileContent(fragFile, nullptr, 0);
        char *fragFileBuffer = new char[fileSize + 1];
        ReadFileContent(fragFile, fragFileBuffer, fileSize);
        fragFileBuffer[fileSize] = '\0';

        auto shader = RenderManager::Instance()->AllocShaderProgram();

        shader->SetVertexShaderSource(vertFileBuffer);
        shader->SetFragmentShaderSource(fragFileBuffer);

        delete[] vertFileBuffer;
        delete[] fragFileBuffer;
        return shader;
    }

    static const char *sectionEnum2String[] = { "Vertex", "Geometry", "TesselationControl", "TesselationEval", "Fragment", "Share", "States" };
    enum SLSectionName
    {
        SLSection_Vertex = 0,
        SLSection_Geometry,
        SLSection_TesselationControl,
        SLSection_TesselationEval,
        SLSection_Fragment,
        SLSection_Share,
        SLSection_States,
        SLSection_Max,
    };

    static const std::unordered_map<std::string, SLSectionName> sectionName2Enum = {
        {sectionEnum2String[0], SLSection_Vertex},
        {sectionEnum2String[1], SLSection_Geometry},
        {sectionEnum2String[2], SLSection_TesselationControl},
        {sectionEnum2String[3], SLSection_TesselationEval},
        {sectionEnum2String[4], SLSection_Fragment},
        {sectionEnum2String[5], SLSection_Share},
        {sectionEnum2String[6], SLSection_States},
    };

    enum TokenType
    {
        TokenType_None,
        TokenType_Include,
        TokenType_Quote,
        TokenType_LeftBrace,
        TokenType_RightBrace,
        TokenType_Name,
    };

    class TinyLexer
    {
    public:
        TinyLexer(const char *src) { mSrc = src; }

        const char *CurrentPtr() { return mSrc; }

        TokenType GetNextToken(const char **tokenStart)
        {
            while(*mSrc != '\0')
            {
                char c = *mSrc;
                // skip white space
                if (c == '\"')
                {
                    *tokenStart = mSrc;
                    mSrc += 1;
                    return TokenType_Quote;
                }
                else if (c == '{')
                {
                    *tokenStart = mSrc;
                    mSrc += 1;
                    return TokenType_LeftBrace;
                }
                else if (c == '}')
                {
                    *tokenStart = mSrc;
                    mSrc += 1;
                    return TokenType_RightBrace;
                }
                else if (c == '#')
                {
                    if (strncmp(mSrc, "#include", 8) == 0)
                    {
                        mSrc += 8;
                        // get file path enclosed within quote
                        while (*mSrc != '\"')
                        {
                            if (*mSrc == '\0' || *mSrc == '\n')
                            {
                                return TokenType_None;
                            }
                            ++mSrc;
                        }
                        
                        *tokenStart = ++mSrc;

                        while (*mSrc != '\"')
                        {
                            if (*mSrc == '\0' || *mSrc == '\n')
                            {
                                return TokenType_None;
                            }
                            ++mSrc;
                        }
                        return TokenType_Include;
                    }
                    else
                        ++mSrc;
                }
                else if (isalpha(c) || isdigit(c))
                {
                    *tokenStart = mSrc;
                    while (isalpha(*mSrc) || isdigit(*mSrc))
                    {
                        ++mSrc;
                    }
                    return TokenType_Name;
                }
                else
                {
                    ++mSrc;
                }
            }

            *tokenStart = nullptr;
            return TokenType_None;
        }

    private:
        const char *mSrc;
    };

    struct TinySLContent
    {
        std::string sectionCode[SLSection_Max];

        void PrintContent()
        {
            for (int sectionId = 0; sectionId < SLSection_Max; ++sectionId)
            {
                RS_LOG_OK_FMT("Section %s:\n%s", sectionEnum2String[sectionId], sectionCode[sectionId].c_str());
            }
        }
    };

    struct ParseContex
    {
        const char *tokenStart;
        TokenType curToken;
        TokenType prevToken = TokenType_None;
        int prevLeftBraceCount = 0;
        const char *sectionStart = nullptr;
        std::string prevName;
        std::string currentSectionName;
    };

    static void ParseTinySL(const std::string &filePath, std::unordered_set<std::string>& includedFiles, std::vector<TinySLContent>& allSLContents)
    {
        size_t fileSize = ReadFileContent(filePath, nullptr, 0);
        if (fileSize == 0)
        {
            RS_LOG_ERROR_FMT("Can't open file: \"%s\"", filePath.c_str());
            return;
        }
        char *sourceBuffer = new char[fileSize + 1];
        ReadFileContent(filePath, sourceBuffer, fileSize);
        sourceBuffer[fileSize] = '\0';

        std::string currentDir;
        GetParentDirectoryFromFilePath(filePath, currentDir);

        TinyLexer lexer(sourceBuffer);
        ParseContex ctx;
        TinySLContent currentContent;

        while ((ctx.curToken = lexer.GetNextToken(&ctx.tokenStart)) != TokenType_None)
        {
            switch (ctx.curToken)
            {
            case TokenType_Include:
                {
                    std::string includeFile(ctx.tokenStart, lexer.CurrentPtr());
                    if (includedFiles.find(includeFile) == includedFiles.end())
                    {
                        includeFile.insert(0, currentDir);
                        ParseTinySL(includeFile, includedFiles, allSLContents);
                    }
                    else
                    {
                        RS_LOG_ERROR_FMT("Duplicated include file: %s", includeFile.c_str());
                    }
                }
                break;
            case TokenType_Quote:
                break;
            case TokenType_LeftBrace:
                if (ctx.prevLeftBraceCount == 0 && ctx.prevToken == TokenType_Name)
                {
                    // new section start
                    ctx.sectionStart = lexer.CurrentPtr();
                    ctx.currentSectionName = ctx.prevName;
                    // RS_LOG_OK_FMT("begin section: %s", ctx.currentSectionName.c_str());
                }
                ++ctx.prevLeftBraceCount;
                break;
            case TokenType_RightBrace:
                if (ctx.prevLeftBraceCount == 1 && ctx.sectionStart != nullptr)
                {
                    // end of section
                    auto iter = sectionName2Enum.find(ctx.currentSectionName);
                    if (iter == sectionName2Enum.end())
                    {
                        RS_LOG_ERROR_FMT("Unrecognized section [%s] in:%s", ctx.currentSectionName.c_str(), filePath.c_str());
                    }
                    else
                    {
                        currentContent.sectionCode[iter->second].assign(ctx.sectionStart, ctx.tokenStart);
                        // RS_LOG_OK_FMT("section content: %s", currentContent.sectionCode[iter->second].c_str());
                    }
                    
                    ctx.sectionStart = nullptr;
                    ctx.currentSectionName = "";
                }
                else if (ctx.prevLeftBraceCount == 0)
                {
                    RS_LOG_ERROR_FMT("Shader brace not match in:%s", filePath.c_str());
                    delete[] sourceBuffer;
                    return;
                }
                --ctx.prevLeftBraceCount;
                break;
            case TokenType_Name:
                ctx.prevName.assign(ctx.tokenStart, lexer.CurrentPtr());
                break;
            }

            ctx.prevToken = ctx.curToken;
        }

        allSLContents.push_back(currentContent);
        delete[] sourceBuffer;
    }

#define NEXT_NAME(name)                                                \
    {                                                                  \
        auto type = lexer.GetNextToken(&tokenStart);                   \
        name.assign(tokenStart, lexer.CurrentPtr());                   \
        if (type != TokenType_Name)                                    \
        {                                                              \
            RS_LOG_ERROR_FMT("Invalid states name: %s", name.c_str()); \
            return;                                                    \
        }                                                              \
    }

#define ON_OFF_OPTION(name, setOption)                               \
    if (name.compare("on") == 0)                                     \
    {                                                                \
        setOption = true;                                            \
    }                                                                \
    else if (name.compare("off") == 0)                               \
    {                                                                \
        setOption = false;                                           \
    }                                                                \
    else                                                             \
    {                                                                \
        RS_LOG_ERROR_FMT("Invalid states option: %s", name.c_str()); \
        return;                                                      \
    }

#define ENUM_OPTIONS(map, name, setOption)                               \
    {                                                                    \
        auto iter = map.find(name);                                      \
        if (iter != map.end())                                           \
        {                                                                \
            setOption = iter->second;                                    \
        }                                                                \
        else                                                             \
        {                                                                \
            RS_LOG_ERROR_FMT("Invalid states option: %s", name.c_str()); \
            return;                                                      \
        }                                                                \
    }

#define NUM_OPTIONS(name, setOption)                                         \
    {                                                                        \
        char *end = nullptr;                                                 \
        setOption = strtol(name.c_str(), &end, 16);                          \
        if (end == nullptr)                                                  \
        {                                                                    \
            setOption = strtol(name.c_str(), &end, 10);                      \
            if (end == nullptr)                                              \
            {                                                                \
                RS_LOG_ERROR_FMT("Invalid states option: %s", name.c_str()); \
                return;                                                      \
            }                                                                \
        }                                                                    \
    }

    enum StateCommand
    {
        StateCommand_ZTest = 0,
        StateCommand_ZWrite,
        StateCommand_ZTestFunc,

        StateCommand_StencilTest,
        StateCommand_StencilMask,
        StateCommand_StencilFunc,
        StateCommand_StencilOp,

        StateCommand_Blend,
        StateCommand_BlendEquation,
        StateCommand_BlendFunc,

        StateCommand_Cull,
        StateCommand_CullFace
    };

    const std::unordered_map<std::string, StateCommand> stateCommands = {
        {"ZTest", StateCommand_ZTest},
        {"ZWrite", StateCommand_ZWrite},
        {"ZTestFunc", StateCommand_ZTestFunc},

        {"Stencil", StateCommand_StencilTest},
        {"StencilMask", StateCommand_StencilMask},
        {"StencilFunc", StateCommand_StencilFunc},
        {"StencilOp", StateCommand_StencilOp},

        {"Blend", StateCommand_Blend},
        {"BlendEquation", StateCommand_BlendEquation},
        {"BlendFunc", StateCommand_BlendFunc},

        {"Cull", StateCommand_Cull},
        {"CullFace", StateCommand_CullFace},
    };

    const std::unordered_map<std::string, DepthStencilFunc> depthStencilFuncOptions = {
        {"always", DepthStencilFunc_Always},
        {"never", DepthStencilFunc_Never},
        {"less", DepthStencilFunc_Less},
        {"greater", DepthStencilFunc_Greater},
        {"equal", DepthStencilFunc_Equal},
        {"notEqual", DepthStencilFunc_NotEqual},
        {"lEqual", DepthStencilFunc_LEqual},
        {"gEqual", DepthStencilFunc_GEqual},
    };

    const std::unordered_map<std::string, StencilOp> stencilOpOptions = {
        {"keep", StencilOp_Keep},
        {"zero", StencilOp_Zero},
        {"replace", StencilOp_Replace},
        {"increase", StencilOp_Increase},
        {"increaseWrap", StencilOp_IncreaseWrap},
        {"decrease", StencilOp_Decrease},
        {"decreaseWrap", StencilOp_DecreaseWrap},
        {"invert", StencilOp_Invert},
    };

    const std::unordered_map<std::string, BlendFunc> blendFuncOptions = {
        {"zero", BlendFunc_Zero},
        {"one", BlendFunc_One},
        {"srcColor", BlendFunc_SrcColor},
        {"oneMinusSrcColor", BlendFunc_OneMinusSrcColor},
        {"dstColor", BlendFunc_DestColor},
        {"oneMinusDstColor", BlendFunc_OneMinusDestColor},
        {"srcAlpha", BlendFunc_SrcAlpha},
        {"oneMinusSrcAlpha", BlendFunc_OneMinusSrcAlpha},
        {"dstAlpha", BlendFunc_DestAlpha},
        {"oneMinusDstAlpha", BlendFunc_OneMinusDestAlpha},
        // constant color support later.
    };
    const std::unordered_map<std::string, BlendEquation> blendEquationOptions = {
        {"add", BlendEquation_Add},
        {"subtract", BlendEquation_Subtract},
        {"reverseSubtract", BlendEquation_ReverseSubTract},
    };

    const std::unordered_map<std::string, CullFace> cullFaceOptions = {
        {"front", CullFace_Front},
        {"back", CullFace_Back},
        {"both", CullFace_FrontAndBack},
    };

    void ParseRenderStates(const std::string &code, RenderStates &states)
    {
        states.Reset();
        TinyLexer lexer(code.c_str());
        const char *tokenStart;

        std::string name;
        while (lexer.GetNextToken(&tokenStart) == TokenType_Name)
        {
            name.assign(tokenStart, lexer.CurrentPtr());
            auto iter = stateCommands.find(name);
            if (iter != stateCommands.end())
            {
                NEXT_NAME(name);
                switch (iter->second)
                {
                case StateCommand_ZTest:
                    ON_OFF_OPTION(name, states.depthTestEnable);
                    break;
                case StateCommand_ZWrite:
                    ON_OFF_OPTION(name, states.depthWriteEnable);
                    break;
                case StateCommand_ZTestFunc:
                    ENUM_OPTIONS(depthStencilFuncOptions, name, states.depthFunc);
                    break;

                case StateCommand_StencilTest:
                    ON_OFF_OPTION(name, states.stencilTestEnable);
                    break;
                case StateCommand_StencilMask:
                    NUM_OPTIONS(name, states.stencilMask);
                    break;
                case StateCommand_StencilFunc:
                    ENUM_OPTIONS(depthStencilFuncOptions, name, states.stencilFunc);
                    NEXT_NAME(name);
                    NUM_OPTIONS(name, states.stencilRef);
                    NEXT_NAME(name);
                    NUM_OPTIONS(name, states.stencilRefMask);
                    break;
                case StateCommand_StencilOp:
                    ENUM_OPTIONS(stencilOpOptions, name, states.stencilFailOp);
                    NEXT_NAME(name);
                    ENUM_OPTIONS(stencilOpOptions, name, states.depthFailOp);
                    NEXT_NAME(name);
                    ENUM_OPTIONS(stencilOpOptions, name, states.depthPassOp);
                    break;

                case StateCommand_Blend:
                    ON_OFF_OPTION(name, states.blendEnable);
                    break;
                case StateCommand_BlendEquation:
                    ENUM_OPTIONS(blendEquationOptions, name, states.blendEquation);
                    break;
                case StateCommand_BlendFunc:
                    ENUM_OPTIONS(blendFuncOptions, name, states.srcBlendFunc);
                    NEXT_NAME(name);
                    ENUM_OPTIONS(blendFuncOptions, name, states.destBlendFunc);
                    break;

                case StateCommand_Cull:
                    ON_OFF_OPTION(name, states.cullingEnable);
                    break;
                case StateCommand_CullFace:
                    ENUM_OPTIONS(cullFaceOptions, name, states.cullFace);
                    break;
                }
            }
            else
            {
                RS_LOG_ERROR_FMT("Unrecognized state command: %s", name.c_str());
            }
        }
    }

    ShaderProgram::SP ShaderUtil::LoadProgramFromTinySL(const std::string &tinyslFile)
    {
        std::vector<TinySLContent> slContents;
        std::unordered_set<std::string> includedFiles;
        // process sections and includes
        ParseTinySL(tinyslFile, includedFiles, slContents);

        if (slContents.size() == 0)
        {
            RS_LOG_ERROR_FMT("Shader parse error: \"%s\"", tinyslFile.c_str());
            return nullptr;
        }

        TinySLContent finalContent;

        // use States section of current file.
        finalContent.sectionCode[SLSection_States].swap(slContents.back().sectionCode[SLSection_States]);
        // merge together
        for (size_t slIdx = 0; slIdx < slContents.size(); ++slIdx)
        {
            for (int sectionId = 0; sectionId < SLSection_Max; ++sectionId)
            {
                if (sectionId != SLSection_States)
                {
                    finalContent.sectionCode[sectionId].append(slContents[slIdx].sectionCode[sectionId]);
                }
            }
        }

        // insert share section to other sections
        finalContent.sectionCode[SLSection_Share].insert(0, versionString);
        for (int sectionId = 0; sectionId < SLSection_Max; ++sectionId)
        {
            if (sectionId != SLSection_States && sectionId != SLSection_Share)
            {
                finalContent.sectionCode[sectionId].insert(0, finalContent.sectionCode[SLSection_Share]);
            }
        }

 //       finalContent.PrintContent();

        RenderStates states;
        ParseRenderStates(finalContent.sectionCode[SLSection_States], states);

        auto shader = RenderManager::Instance()->AllocShaderProgram();
        shader->SetVertexShaderSource(finalContent.sectionCode[SLSection_Vertex].c_str());
        shader->SetFragmentShaderSource(finalContent.sectionCode[SLSection_Fragment].c_str());
        shader->SetStates(states);
        return shader;
    }

    void ShaderUtil::SetTinySLVersionString(const std::string &version)
    {
        versionString = version;
    }
}