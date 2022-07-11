#pragma

#include "IRuntimeModule.h"

namespace Application
{
    Interface IApplication : implements IRuntimeModule
    {
    public:
        virtual int Initialize() override = 0;
        virtual void Finalize() override = 0;
        virtual void Tick() override = 0;

        virtual bool IsQuit() = 0;
    };
}; // namespace Application