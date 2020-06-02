#include "BaseApplication.hpp"

namespace My
{
    BaseApplication app;
    extern IApplication *g_app = &app;
} // namespace My