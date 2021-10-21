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

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev)
{
    ctxErrorOccurred = true;
    return 0;
}

void DrawQuad()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.f, 1.f, -1.f, 1.f, 1.f, 20.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.f, 0.f, 10.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

    glBegin(GL_QUADS);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-0.75f, -0.75f, 0);

    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.75f, -0.75f, 0.f);

    glColor3f(0, 0, 1);
    glVertex3f(0.75f, 0.75f, 0);

    glColor3f(1.f, 1.f, 0.f);
    glVertex3f(-0.75f, 0.75f, 0.);
    glEnd();
}

int main(void)
{
    xcb_connection_t *pConn;
    xcb_screen_t *pScreen;
    xcb_window_t window;
    xcb_gcontext_t foreground;
    xcb_gcontext_t background;
    xcb_generic_event_t *pEvent;
    xcb_colormap_t colormap;
    uint32_t mask = 0;
    uint32_t values[3];
    uint8_t isQuit = 0;

    char title[] = "Hello, engine![OpenGL]";
    char title_icon[] = "Hello, Engine!(iconified)";

    Display *display;
    int default_screen;
    GLXContext context;
    GLXFBConfig *fb_configs;
    GLXFBConfig fb_config;
    int num_fb_configs = 0;
    XVisualInfo *vi;

    GLXDrawable drawable;
    GLXWindow glxwindow;
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
    const char *glxExts;

    // Get a matching FB config
    static int visual_attribs[] =
        {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, True,
            // GLX_SAMPLE_BUFFERS, 1,
            // GLX_SAMPLES, 4,
            None};
    int glx_major, glx_minor;

    /* Open Xlib Display */
    display = XOpenDisplay(NULL);
    if (!display)
    {
        fprintf(stderr, "Can't open display\n");
        return -1;
    }

    if (!glXQueryVersion(display, &glx_major, &glx_minor) || ((glx_major == 1) && (glx_minor < 3) || (glx_major < 1)))
    {
        fprintf(stderr, "Invalid GLX version\n");
        return -1;
    }

    default_screen = DefaultScreen(display);
    /* Query framebuffer configuration */
    fb_configs = glXChooseFBConfig(display, default_screen, visual_attribs, &num_fb_configs);
    if (!fb_configs || num_fb_configs == 0)
    {
        fprintf(stderr, "glXGetFBConfigs failed\n");
        return -1;
    }

    /* Pick the FB config/visual with the most samples per pixel */
    {
        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

        for (int i = 0; i < num_fb_configs; ++i)
        {
            XVisualInfo *vi = glXGetVisualFromFBConfig(display, fb_configs[i]);
            if (vi)
            {
                int samp_buf, samples;
                glXGetFBConfigAttrib(display, fb_configs[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(display, fb_configs[i], GLX_SAMPLES, &samples);

                printf( " Matching fbconfig %d, visual ID 0x%lx: SAMPLE_BUFFERS = %d,"
                        " SAMPLES = %d\n",
                        i, vi->visualid, samp_buf, samples);
                if (best_fbc < 0 || (samp_buf && samples > best_num_samp))
                    best_fbc = i, best_num_samp = samples;
                if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                    worst_fbc = i, worst_num_samp = samples;
            }
            XFree( vi );
        }

        fb_config = fb_configs[best_fbc];
    }

    /* Get a visual */
    vi = glXGetVisualFromFBConfig(display, fb_config);
    
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