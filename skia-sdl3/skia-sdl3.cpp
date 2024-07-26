#define SK_GANESH
#define SK_GL
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/gl/GrGLAssembleInterface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkSurface.h"

#include "include/gpu/ganesh/gl/egl/GrGLMakeEGLInterface.h"
//#include "include/gpu/ganesh/gl/glx/GrGLMakeGLXInterface.h"

//#include "include/gpu/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "src/gpu/ganesh/gl/GrGLDefines.h"

//#include "tools/fonts/FontToolUtils.h"
#include "include/ports/SkFontMgr_fontconfig.h"


#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>


//uncomment the two lines below to enable correct color spaces
//#define GL_FRAMEBUFFER_SRGB 0x8DB9
//#define GL_SRGB8_ALPHA8 0x8C43

GrDirectContext* sContext = nullptr;
SkSurface* sSurface = nullptr;

static sk_sp<SkFontMgr> fontMgr;
static sk_sp<SkTypeface> tf;

SDL_GLContext glContext = nullptr;

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GL_TRUE);
//}

int init_skia(int w, int h) {

	//auto interface = GrGLMakeNativeInterface();
	//if (interface == nullptr) {
	//	//backup plan. see https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add?permalink_comment_id=4680136#gistcomment-4680136
	//	interface = GrGLMakeAssembledInterface(
	//		nullptr, (GrGLGetProc) * [](void*, const char* p) -> void* { return (void*)glfwGetProcAddress(p); });
	//}

        //auto interface = GrGLInterfaces::MakeGLX();
        auto interface = GrGLInterfaces::MakeEGL();

	sContext = GrDirectContexts::MakeGL(interface).release();

	GrGLFramebufferInfo framebufferInfo;

        // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
        // render to it
        GrGLint buffer;
        glGetIntegerv( GR_GL_FRAMEBUFFER_BINDING, &buffer);
        GrGLFramebufferInfo info;
        info.fFBOID = (GrGLuint) buffer;
        printf("FB id = %d\n", buffer);

	//framebufferInfo.fFBOID = 0; // assume default framebuffer
	// We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
	//(replace line below with this one to enable correct color spaces) framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
	framebufferInfo.fFormat = GR_GL_RGBA8;

	SkColorType colorType = kRGBA_8888_SkColorType;
	GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(w, h,
		0, // sample count
		0, // stencil bits
		framebufferInfo);

	//(replace line below with this one to enable correct color spaces) sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, SkColorSpace::MakeSRGB(), nullptr).release();
	sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();
	if (sSurface == nullptr) abort();


        //tf = fontMgr->matchFamilyStyle("sans-serif", SkFontStyle());
        //tf = fontMgr->makeFromFile("DejaVuSans.ttf", 0);  //ToolUtils::CreatePortableTypeface("serif", SkFontStyle());
        //if (tf == nullptr) {
        //    printf("Font not found\n");
        //    return SDL_APP_FAILURE;
        //}
        return SDL_APP_SUCCESS;
}

void cleanup_skia() {
	delete sSurface;
	delete sContext;
}

//////////////////////////////////////////////////////


/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
//static SDL_Renderer *renderer = NULL;

/* This function runs once at startup. */
int SDL_AppInit(void **appstate, int argc, char *argv[])
{
	fontMgr = SkFontMgr_New_FontConfig(nullptr);
	//fontMgr = SkFontMgr::RefEmpty();


    printf("SDL_AppInit()\n");
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
        printf("1\n");
        return SDL_APP_FAILURE;
    }

//    if (SDL_CreateWindowAndRenderer("examples/renderer/clear", 1280, 720, 0, &window, &renderer) == -1) {
//        printf("2. Error: %s\n", SDL_GetError());
//        return SDL_APP_FAILURE;
//    }
//    SDL_SetRenderVSync(renderer, 1);  /* try to show frames at the monitor refresh rate. */


    window = SDL_CreateWindow("SDL Window", 1280, 720, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        printf("2. Error: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // try and setup a GL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        //handle_error();
        printf("context creation error!\n");
        return SDL_APP_FAILURE;
    }

    int success =  SDL_GL_MakeCurrent(window, glContext);
    if (success != 0) {
        //handle_error();
        printf("can't sec context current!\n");
        return SDL_APP_FAILURE;
    }

    uint32_t windowFormat = SDL_GetWindowPixelFormat(window);
    int contextType;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    int dw, dh;
    //SDL_GL_GetDrawableSize(window, &dw, &dh)
    dw=1280; dh=720;
    printf("dw=%d, dh=%d\n", dw, dh);

    glViewport(0, 0, dw, dh);

    success = init_skia(1280, 720);
    if (success == SDL_APP_FAILURE) return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
int SDL_AppEvent(void *appstate, const SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN) {
        printf("key: %d, esc: %d\n", event->key.scancode, SDL_SCANCODE_ESCAPE);
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* the current red color we're clearing to. */
static int red = 0, green = 0, blue = 0;
/* When fading up, this is 1, when fading down, it's -1. */
static int red_fade_dir = 1;
static int green_fade_dir = 1;
static int blue_fade_dir = 1;

/* This function runs once per frame, and is the heart of the program. */
int SDL_AppIterate(void *appstate)
{
    /* update the color for the next frame we will draw. */
    if (red_fade_dir > 0) {
        if (red == 255) {
            red_fade_dir = -1;
        } else {
            red+=4;
        }
    } else if (red_fade_dir < 0) {
        if (red == 0) {
            red_fade_dir = 1;
        } else {
            red-=3;
        }
    }

    if (green_fade_dir > 0) {
        if (green == 255) {
            green_fade_dir = -1;
        } else {
            green+=2;
        }
    } else if (green_fade_dir < 0) {
        if (green == 0) {
            green_fade_dir = 1;
        } else {
            green-=2;
        }
    }

    if (blue_fade_dir > 0) {
        if (blue == 255) {
            blue_fade_dir = -1;
        } else {
            blue+=1;
        }
    } else if (blue_fade_dir < 0) {
        if (blue == 0) {
            blue_fade_dir = 1;
        } else {
            blue-=2;
        }
    }

    if (red > 255) red = 255;
    if (red < 0) red = 0;
    if (green > 255) green = 255;
    if (green < 0) green = 0;
    if (blue > 255) blue = 255;
    if (blue < 0) blue = 0;

    //glClearColor(red, green, blue, 1);
    //glClearStencil(0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    SkPaint paint, paint2;
    auto tf = fontMgr->matchFamilyStyle("sans-serif", SkFontStyle());
    if (tf == nullptr) {
        printf("No typeface\n");
        return SDL_APP_FAILURE;
    }
    //auto tf = SkFontMgr::RefEmpty()->makeFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0);  //ToolUtils::CreatePortableTypeface("serif", SkFontStyle());

    //auto tf = fontMgr->makeFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0);  //ToolUtils::CreatePortableTypeface("serif", SkFontStyle());

    SkCanvas* canvas = sSurface->getCanvas();
    //canvas->scale((float)dw/dm.w, (float)dh/dm.h);
    canvas->clear(SK_ColorBLUE);

    paint2.setColor(SK_ColorRED);
    canvas->drawRect({100, 200, 400, 300}, paint2);

    SkFont font(tf, 32);
    paint.setColor(SK_ColorWHITE);
    canvas->drawString("This is a string!!! It is printed, really!!!!", 50.0f, 30.0f, font, paint);


    sContext->flushAndSubmit();


    /* since we're always fading red, we leave green and blue at zero.
       alpha doesn't mean much here, so leave it at full (255, no transparency). */
    //SDL_SetRenderDrawColor(renderer, red, green, blue, 255);

    /* clear the window to the draw color. */
    //SDL_RenderClear(renderer);

    /* put the newly-cleared rendering on the screen. */
    //SDL_RenderPresent(renderer);
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate)
{
    /* SDL will clean up the window/renderer for us. */
}
