#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include "xcb/xcb.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

// Helper to check for extension string presence. Adapted from:
// http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    /* It takes a bit of care to be fool-proof about parsing the
       OpenGL extensions string. Don't be fooled by sub-strings,
       etc. */
    for (start = extList;;)
    {
        where = strstr(start, extension);

        if (!where)
            break;
        terminator = where + strlen(extension);
        if (where == start || *(where - 1) == ' ')
        {
            if (*terminator == ' ' || *terminator == '\0')
                return true;
        }

        start = terminator;
    }

    return false;
}

int main(void)
{
    xcb_connection_t *pConn;
    xcb_screen_t *pScreen;
    xcb_window_t window;
    xcb_gcontext_t foreground;
    xcb_gcontext_t background;
    xcb_generic_event_t *pEvent;
    uint32_t mask = 0;
    uint32_t values[2];
    uint8_t isQuit = 0;

    char title[] = "Hello, engine!";
    char title_icon[] = "Hello, Engine!(iconified)";

    pConn = xcb_connect(0, 0);

    pScreen = xcb_setup_roots_iterator(xcb_get_setup(pConn)).data;

    window = pScreen->root;

    foreground = xcb_generate_id(pConn);
    mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = pScreen->black_pixel;
    values[1] = 0;
    xcb_create_gc(pConn, foreground, window, mask, values);

    background = xcb_generate_id(pConn);
    mask = XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = pScreen->white_pixel;
    values[1] = 0;
    xcb_create_gc(pConn, background, window, mask, values);

    window = xcb_generate_id(pConn);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = pScreen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

    xcb_create_window(pConn,
                      XCB_COPY_FROM_PARENT,
                      window,
                      pScreen->root, 20, 20,
                      640, 480,
                      10,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      pScreen->root_visual,
                      mask, values);

    xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                        strlen(title), title);
    xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
                        XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8,
                        strlen(title_icon), title_icon);

    xcb_map_window(pConn, window);

    xcb_flush(pConn);

    while ((pEvent = xcb_wait_for_event(pConn)) && !isQuit)
    {
        switch (pEvent->response_type & ~0x80)
        {
        case XCB_EXPOSE:
            break;

        case XCB_KEY_PRESS:
            isQuit = 1;
            break;
        }
        free(pEvent);
    }

    return 0;
}