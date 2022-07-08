#include "WindowApplication.h"

namespace My
{
    int WindowApplication::Initialize()
    {
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window)
            return -1;
        
        glfwMakeContextCurrent(m_window);
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
        
    }

    bool WindowApplication::IsQuit()
    {
        return m_bQuit;
    }
}