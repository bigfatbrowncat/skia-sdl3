#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>

#include "GraphApp.h"
#include "SDLGraphAppBase.h"

#include <SDL3/SDL_main.h>

using namespace std;

/* This function runs once at startup. */
int SDL_AppInit(void **appstate, int argc, char *argv[])
{
  try {
    *appstate = (void*)(new GraphApp());
    return SDL_APP_CONTINUE;  /* carry on with the program! */
  } catch (const runtime_error& e) {
    std::cerr << "Unrecoverable initialization error." << endl;
    std::cerr << e.what() << endl;
    std::cerr << "Program halted." << endl;
    return SDL_APP_FAILURE;
  }
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
int SDL_AppEvent(void *appstate, const SDL_Event *event)
{
  try {
    SDLGraphAppBase& app = *(SDLGraphAppBase*)appstate;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        cout << "Key scancode: " << event->key.scancode << endl;
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
  } catch (const runtime_error& e) {
    std::cerr << "Unrecoverable error during an event processing." << endl;
    std::cerr << e.what() << endl;
    std::cerr << "Program halted." << endl;
    return SDL_APP_FAILURE;
  }
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
  try {
    GraphApp& app = *(GraphApp*)appstate;

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

    app.onLoop();
    app.commitDrawing();


    /* since we're always fading red, we leave green and blue at zero.
       alpha doesn't mean much here, so leave it at full (255, no transparency). */
    //SDL_SetRenderDrawColor(renderer, red, green, blue, 255);

    /* clear the window to the draw color. */
    //SDL_RenderClear(renderer);

    /* put the newly-cleared rendering on the screen. */
    //SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
  } catch (const runtime_error& e) {
    std::cerr << "Unrecoverable error occurred in the main loop." << endl;
    std::cerr << e.what() << endl;
    std::cerr << "Program halted." << endl;
    return SDL_APP_FAILURE;
  }
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate)
{
    GraphApp* app = (GraphApp*)appstate;
    delete app;

    /* SDL will clean up the window/renderer for us. */
}

/////////////////////////////////////////////////////////////////////////

//uncomment the two lines below to enable correct color spaces
//#define GL_FRAMEBUFFER_SRGB 0x8DB9
//#define GL_SRGB8_ALPHA8 0x8C43

void SDLGraphAppBase::throwSDLError(const string& msg) {
  stringstream ss;
  ss << msg << " SDL error: " << SDL_GetError();
  throw runtime_error(ss.str());
}

void SDLGraphAppBase::createFontMgr() {
  fontMgr = SkFontMgr_New_FontConfig(nullptr);
  int families = fontMgr->countFamilies();
  cout << "Font families count: " << families << endl;
}

void SDLGraphAppBase::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      throwSDLError("Couldn't initialize SDL!");
  }
}

WH SDLGraphAppBase::getScreenSize() {
  int count;
  const SDL_DisplayID* ids = SDL_GetDisplays(&count);
  if (ids == nullptr) throwSDLError("Can not get the displays info.");
  if (count < 1) {
    throw runtime_error("No displays found");
  }

  const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(ids[0]);
  if (dm == 0) throwSDLError("Can not get screen size.");
  return WH { dm->w, dm->h };
}

void SDLGraphAppBase::createSDLWindowAndContext() {

  window = SDL_CreateWindow("SDL Window", 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
  if (window == nullptr) {
    throwSDLError("Can't create a window.");
  }

  // try and setup a GL context
  glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    throwSDLError("Can't create context.");
  }
}

void SDLGraphAppBase::makeGLContextCurrent(int w, int h) {

  int success =  SDL_GL_MakeCurrent(window, glContext);
  if (success != 0) {
    throwSDLError("Can't make the context current.");
  }

  //uint32_t windowFormat = SDL_GetWindowPixelFormat(window);
  //int contextType;
  //SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);
}

void SDLGraphAppBase::createSkiaContext() {
  auto interface = GrGLInterfaces::MakeEGL();
  sContext = GrDirectContexts::MakeGL(interface);
}

void SDLGraphAppBase::createSkiaSurface(int w, int h) {
  GrGLFramebufferInfo framebufferInfo;

  // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
  // render to it
  GrGLint buffer;
  glGetIntegerv( GR_GL_FRAMEBUFFER_BINDING, &buffer);

  // We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
  //(replace line below with this one to enable correct color spaces) framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
  framebufferInfo.fFormat = GR_GL_RGBA8;
  framebufferInfo.fFBOID = (GrGLuint) buffer;

  SkColorType colorType = kRGBA_8888_SkColorType;
  GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(w, h,
      0, // sample count
      0, // stencil bits
      framebufferInfo);

  //(replace line below with this one to enable correct color spaces) sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, SkColorSpace::MakeSRGB(), nullptr).release();
  GrRecordingContext* recContext = dynamic_cast<GrRecordingContext*>(sContext.get());
  sSurface = SkSurfaces::WrapBackendRenderTarget(recContext,
      backendRenderTarget,
      kBottomLeft_GrSurfaceOrigin,
      colorType,
      nullptr,
      nullptr);

  if (sSurface == nullptr) {
    throw runtime_error("Can not create Skia surface.");
  }
}

SDLGraphAppBase::SDLGraphAppBase() {
  createFontMgr();
  initSDL();
  createSDLWindowAndContext();
  WH scrSz = getScreenSize();
  makeGLContextCurrent(scrSz.w, scrSz.h);
  createSkiaContext();
  createSkiaSurface(scrSz.w, scrSz.h);
}

SDLGraphAppBase::~SDLGraphAppBase() {
  sSurface = nullptr;
  sContext = nullptr;
}

sk_sp<SkTypeface> SDLGraphAppBase::getTypeface(const string& name) {
  //auto tf = SkFontMgr::RefEmpty()->makeFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0);  //ToolUtils::CreatePortableTypeface("serif", SkFontStyle());
  //auto tf = fontMgr->makeFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0);  //ToolUtils::CreatePortableTypeface("serif", SkFontStyle());
  auto tf = fontMgr->matchFamilyStyle("sans-serif", SkFontStyle());
  if (tf == nullptr) {
    throw runtime_error(string("No typeface found") + name);
  }
  return tf;
}

SkCanvas* SDLGraphAppBase::getCanvas() {
  SkCanvas* canvas = sSurface->getCanvas();
  if (canvas == nullptr) {
    throw runtime_error(string("Can not get a canvas for the surface"));
  }
  return canvas;
}

void SDLGraphAppBase::commitDrawing() {
  sContext->flushAndSubmit();
  SDL_GL_SwapWindow(window);
}

