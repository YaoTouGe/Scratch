#pragma once

#include "../Interface/IApplication.hpp"

namespace My
{
    class BaseApplication: implements IApplication
    {
    public:
        virtual int Initialize() override;
        virtual void Finalize() override;
        virtual void Tick() override;

        virtual bool IsQuit();

    protected:
        bool m_bQuit;
    }

    extern BaseApplication* g_app;
}
