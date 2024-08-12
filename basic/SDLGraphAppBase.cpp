#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>

#include "GraphAppImpl.h"
#include "SDLGraphAppBase.h"
#include "SDL_oldnames.h"

#include <SDL3/SDL_main.h>

using namespace std;

#define TURN_SCREEN 1

#ifndef SDL_APP_CONTINUE
#  define SDL_APP_CONTINUE 0
#  define SDL_APP_FAILURE -1
#  define SDL_APP_SUCCESS 1
#endif

/* This function runs once at startup. */
int appInit(GraphAppCont **appstate, GraphAppCallbacks* cb)
{
  try {
    *appstate = new GraphAppCont;
    (*appstate)->impl = new GraphAppImpl(cb);
    (*appstate)->impl->cb->onInit(*appstate);
    return SDL_APP_CONTINUE;  /* carry on with the program! */
  } catch (const runtime_error& e) {
    std::cerr << "Unrecoverable initialization error." << endl;
    std::cerr << e.what() << endl;
    std::cerr << "Program halted." << endl;
    return SDL_APP_FAILURE;
  }
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
int appEvent(GraphAppCont *appstate, const SDL_Event *event)
{
  try {
    SDLGraphAppBase& app = *(dynamic_cast<SDLGraphAppBase*>(appstate->impl));

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

/* This function runs once per frame, and is the heart of the program. */
int appIterate(GraphAppCont *appstate)
{
  try {
    GraphAppImpl* app = appstate->impl;

    app->cb->onLoop(appstate);
    app->commitDrawing();


    return SDL_APP_CONTINUE;  /* carry on with the program! */
  } catch (const runtime_error& e) {
    std::cerr << "Unrecoverable error occurred in the main loop." << endl;
    std::cerr << e.what() << endl;
    std::cerr << "Program halted." << endl;
    return SDL_APP_FAILURE;
  }
}

/* This function runs once at shutdown. */
void appQuit(GraphAppCont *appstate)
{
  appstate->impl->cb->onFin(appstate);
  delete appstate->impl;
  delete appstate;

  /* SDL will clean up the window/renderer for us. */
}

extern "C" int GraphApp_main(GraphAppCallbacks* cb) {

  GraphAppCont* appstate = nullptr;

  auto res = appInit(&appstate, cb);
  GraphAppImpl* app = appstate->impl;

  IntSize sz = app->getScreenSize();

  //auto res = SDL_APP_CONTINUE;
  SDL_Event event;
  while (res == SDL_APP_CONTINUE) {
    auto prev_time = SDL_GetTicks();

    if (TURN_SCREEN) {
      app->getCanvas()->translate(sz.height, 0);
      app->getCanvas()->rotate(90);
    }
    res = appIterate(appstate);
    if (TURN_SCREEN) {
      app->getCanvas()->rotate(-90);
      app->getCanvas()->translate(-sz.height, 0);
    }

    int FPS = appstate->impl->getFPS();
    auto mspf = 0;
    if (FPS > 0) mspf = 1000 / FPS;


    if (FPS > 0) {
      while (SDL_GetTicks() - prev_time < mspf && res == SDL_APP_CONTINUE) {
        SDL_WaitEventTimeout(&event, mspf - (SDL_GetTicks() - prev_time));
        res = appEvent(appstate, &event);
      }
    } else {
      SDL_WaitEvent(&event);
      res = appEvent(appstate, &event);
    }
  }

  appQuit(appstate);
  return res;
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


std::shared_ptr<SDL_DisplayID> SDLGraphAppBase::getMainDisplay() {
  int num_displays = 0;
  std::shared_ptr<SDL_DisplayID> res = nullptr;
  const SDL_DisplayID *displays = SDL_GetDisplays(&num_displays);
  if (displays) {
    cout << "Displays: " << std::endl;
    for (int i = 0; i < num_displays; i++) {
      SDL_DisplayID instance_id = displays[i];
      const char *name = SDL_GetDisplayName(instance_id);
      cout << "Display #" << instance_id << " : ";
      if (name != nullptr) {
        cout << name;
      } else {
        cout << "No name";
      }
      cout << endl;

      int num_modes;
      const SDL_DisplayMode * const* modes = SDL_GetFullscreenDisplayModes(instance_id, &num_modes);
      cout << " - Modes:" << endl;
      if (modes) {
        for (int i = 0; i < num_modes; i++) {
          const SDL_DisplayMode *mode = modes[i];
          cout << "   - " << mode->w << "x" << mode->h << " @ " << mode->refresh_rate << ", density = " << mode->pixel_density << endl;
        }
        //SDL_CleanupTemporaryMemory((void*)modes); //-- !!!
      }

      SDL_Rect bounds;
      SDL_GetDisplayBounds( instance_id, &bounds );
      cout << " - Bounds: " << bounds.x << ", " << bounds.y << ", " << bounds.w << ", " << bounds.h << std::endl;

      //SDL_DisplayData *pData =(SDL_DisplayData*)SDL_GetDisplayDriverData(displayIndex);

      if (num_modes == 1 && modes[0]->w == 720 && modes[0]->h == 1280) {
        res = std::make_shared<SDL_DisplayID>(instance_id);
      }
    }
    //SDL_CleanupTemporaryMemory((void*)displays);
  }
  return res;
}

void SDLGraphAppBase::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    throwSDLError("Couldn't initialize SDL!");
  }
}

IntSize SDLGraphAppBase::getScreenSize() {
  auto main_display_id = getMainDisplay();
  if (main_display_id == nullptr) {
    throw std::runtime_error("Can not find the standard display");
  }

  SDL_Rect display_bounds;
  SDL_GetDisplayBounds( *main_display_id, &display_bounds );


  // int count;
  // const SDL_DisplayID* ids = SDL_GetDisplays(&count);
  // if (ids == nullptr) throwSDLError("Can not get the displays info.");
  // if (count < 1) {
  //   throw runtime_error("No displays found");
  // }

  // const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(ids[0]);
  // if (dm == 0) throwSDLError("Can not get screen size.");

  if (TURN_SCREEN) {
    return IntSize { display_bounds.h, display_bounds.w };
  } else {
    return IntSize { display_bounds.w, display_bounds.h };
  }
}

void SDLGraphAppBase::createSDLWindowAndContext() {

  auto main_display_id = getMainDisplay();
  if (main_display_id == nullptr) {
    throw std::runtime_error("Can not find the standard display");
  }

  SDL_Rect display_bounds;
  SDL_GetDisplayBounds( *main_display_id, &display_bounds );

  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetStringProperty(props, "title", "SDL window");
  SDL_SetNumberProperty(props, "x", display_bounds.x);
  SDL_SetNumberProperty(props, "y", display_bounds.y);
  SDL_SetNumberProperty(props, "width", display_bounds.w);
  SDL_SetNumberProperty(props, "height", display_bounds.h);
  SDL_SetNumberProperty(props, "flags", SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN*/);
  window = SDL_CreateWindowWithProperties(props);
  SDL_DestroyProperties(props);

  if (window == nullptr) {
    throwSDLError("Can't create a window.");
  }

  // try and setup a GL context
  glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    throwSDLError("Can't create context.");
  }
}

void SDLGraphAppBase::makeGLContextCurrent() {

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
  IntSize scrSz = getScreenSize();

  makeGLContextCurrent();

  createSkiaContext();


  if (TURN_SCREEN) {
    createSkiaSurface(scrSz.height, scrSz.width);
  } else {
    createSkiaSurface(scrSz.width, scrSz.height);
  }
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

