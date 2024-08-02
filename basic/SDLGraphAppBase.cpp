#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>

#include "GraphAppImpl.h"
#include "SDLGraphAppBase.h"

#include <SDL3/SDL_main.h>

using namespace std;

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

//struct GraphAppCallbacks {
//  init_callback* onInit;
//  loop_callback* onLoop;
//  fin_callback* onFin;
//};

extern "C" int GraphApp_main(GraphAppCallbacks* cb) {

  GraphAppCont* appstate = nullptr;
  auto res = appInit(&appstate, cb);

  //auto res = SDL_APP_CONTINUE;
  SDL_Event event;
  while (res == SDL_APP_CONTINUE) {
    auto prev_time = SDL_GetTicks();
    res = appIterate(appstate);

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

void SDLGraphAppBase::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      throwSDLError("Couldn't initialize SDL!");
  }
}

IntSize SDLGraphAppBase::getScreenSize() {
  int count;
  const SDL_DisplayID* ids = SDL_GetDisplays(&count);
  if (ids == nullptr) throwSDLError("Can not get the displays info.");
  if (count < 1) {
    throw runtime_error("No displays found");
  }

  const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(ids[0]);
  if (dm == 0) throwSDLError("Can not get screen size.");
  return IntSize { dm->w, dm->h };
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
  IntSize scrSz = getScreenSize();
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

