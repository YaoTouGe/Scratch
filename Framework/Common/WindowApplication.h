#pragma once

#include <string>
#include "IApplication.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace My
{
    class WindowApplication : implements IApplication
    {
    public:
        WindowApplication(int width, int height, const char* title)
        {
            m_width = width;
            m_height = height;
            m_title = title;
        }

        virtual int Initialize() override;
        virtual void Finalize() override;
        virtual void Tick() override;
        virtual bool IsQuit() override;

        virtual void Render() = 0;
        virtual void KeyBoard(char key) = 0;

    protected:
        bool m_bQuit = false;
        GLFWwindow* m_window = nullptr;

        int m_width;
        int m_height;
        std::string m_title;
    };
} // namespace My