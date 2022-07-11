#pragma once

#include <string>
#include "IApplication.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace Application
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
        void ShowAndRun();

        virtual void Render() = 0;
        virtual void KeyBoard(int key, int action) = 0;
        virtual void MouseMove(int x, int y) = 0;
        virtual void MouseButton(int button, int action, int mods, int x, int y) = 0;

    protected:
        static void keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void mouseMoveCallBack(GLFWwindow *window, double xpos, double ypos);
        static void mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods);

        bool m_bQuit = false;
        GLFWwindow* m_window = nullptr;

        int m_width;
        int m_height;
        std::string m_title;

        bool m_IsDrag;
        int m_MouseType = -1;
        int m_PrevX, m_PrevY;
        int m_DisplayTimeout;
    };
} // namespace Application