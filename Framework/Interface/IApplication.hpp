#pragma

#include "IRuntimeModule.hpp"

namespace My
{
    Interface IApplication: implements IRuntimeModule
    {
    public:
        virtual int Initialize() = 0 override;
        virtual void Finalize() = 0 override;
        virtual void Tick() = 0 override;

        virtual bool IsQuit() = 0;
    }
}