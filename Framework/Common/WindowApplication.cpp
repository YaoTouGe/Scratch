#include "WindowApplication.h"

namespace Application
{
    int WindowApplication::Initialize()
    {
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);

        if (!m_window)
            return -1;
        
        glfwMakeContextCurrent(m_window);
        glfwSetKeyCallback(m_window, &WindowApplication::keyCallBack);
        glfwSetCursorPosCallback(m_window, &WindowApplication::mouseMoveCallBack);
        glfwSetMouseButtonCallback(m_window, &WindowApplication::mouseButtonCallBack);

        glewInit();
        return 0;
    }
    void WindowApplication::Finalize()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void WindowApplication::Tick()
    {
        Render();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    bool WindowApplication::IsQuit()
    {
        return m_bQuit;
    }

    void WindowApplication::ShowAndRun()
    {
        while(!m_bQuit && !glfwWindowShouldClose(m_window))
        {
            Tick();
        }
    }

    void WindowApplication::keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        auto app = (WindowApplication*)glfwGetWindowUserPointer(window);
        app->KeyBoard(key, action);
    }

    void WindowApplication::mouseMoveCallBack(GLFWwindow *window, double xpos, double ypos)
    {
        auto app = (WindowApplication *)glfwGetWindowUserPointer(window);
        app->MouseMove((int)xpos, (int)ypos);
    }
    void WindowApplication::mouseButtonCallBack(GLFWwindow *window, int button, int action, int mods)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        auto app = (WindowApplication *)glfwGetWindowUserPointer(window);
        app->MouseButton(button, action, mods, (int)x, (int)y);
    }
}