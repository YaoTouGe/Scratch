#include <stdio.h>

#include "IApplication.h"

using namespace Application;

namespace Application
{
    extern IApplication *g_app;
}

int main(int argc, char **argv)
{
    int ret;

    if ((ret = g_app->Initialize()) != 0)
    {
        printf("App initialize failed, will exit now.");
        return ret;
    }

    while (!g_app->IsQuit())
    {
        g_app->Tick();
    }

    g_app->Finalize();
    return 0;
}