#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>

#include "GraphAppImpl.h"
#include "SDLGraphAppBase.h"
#include "SDL_oldnames.h"
#include "SDL_video.h"

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

extern "C" int GraphApp_main(GraphAppCallbacks* cb) {

  GraphAppCont* appstate = nullptr;

  auto res = appInit(&appstate, cb);
  GraphAppImpl* app = appstate->impl;

  //IntSize sz = app->getScreenSize();
  std::vector<std::pair<SDL_DisplayID, DisplayInfo>> displays = app->getDisplays();

  //auto res = SDL_APP_CONTINUE;
  SDL_Event event;
  while (res == SDL_APP_CONTINUE) {
    auto prev_time = SDL_GetTicks();

    for (uint8_t current_display_index = 0; current_display_index < displays.size(); current_display_index ++) {
      app->setCurrentDisplayIndex(current_display_index);
      app->makeGLContextCurrent(app->windowsAndContexts[app->getCurrentDisplayIndex()]);

      const auto& display_info = displays[current_display_index].second;
      const IntSize& sz = display_info.size;
      DisplayOrientation orientation = display_info.orientation;

      printf("index: %d sz: %d, %d\n", current_display_index, sz.width, sz.height);
      if (orientation == DISPLAY_ORIENTATION_LEFT_VERTICAL) {
        app->getCanvas()->translate(sz.width, 0);
        app->getCanvas()->rotate(90);
      }

      res = appIterate(appstate);

      if (orientation == DISPLAY_ORIENTATION_LEFT_VERTICAL) {
        app->getCanvas()->rotate(-90);
        app->getCanvas()->translate(-sz.width, 0);
      }
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

std::vector<std::pair<SDL_DisplayID, DisplayInfo>> SDLGraphAppBase::getDisplays() {
  std::vector<std::pair<SDL_DisplayID, DisplayInfo>> res;

  int num_displays = 0;
  //std::shared_ptr<SDL_DisplayID> res = nullptr;
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

      DisplayInfo display_info;
      display_info.size = { 0, 0 };

      int num_modes;
      const SDL_DisplayMode * const* modes = SDL_GetFullscreenDisplayModes(instance_id, &num_modes);
      cout << " - Modes:" << endl;
      if (modes) {
        for (int i = 0; i < num_modes; i++) {
          const SDL_DisplayMode *mode = modes[i];
          cout << "   - " << mode->w << "x" << mode->h << " @ " << mode->refresh_rate << ", density = " << mode->pixel_density << endl << flush;

          // If the mode is smaller or equal to 4K and
          // it's bigger than the current one, setting it as our main
          if ((uint64_t)display_info.size.width * (uint64_t)display_info.size.height < (uint64_t)mode->w * (uint64_t)mode->h &&
               (uint64_t)mode->w * (uint64_t)mode->h <= 3840 * 2160) {
            display_info.size.width = mode->w;
            display_info.size.height = mode->h;
          }
          //  else {
          //   if (mode->w > mode->h) {
          //     display_info.size.width = 3840;
          //     display_info.size.height = (uint64_t)3840 * mode->h / mode->w;
          //   } else {
          //     display_info.size.height = 3840;
          //     display_info.size.width = (uint64_t)3840 * mode->w / mode->h;
          //   }
          // }
        }
      }

      SDL_Rect bounds;
      SDL_GetDisplayBounds( instance_id, &bounds );
      cout << " - Bounds: " << bounds.x << ", " << bounds.y << ", " << bounds.w << ", " << bounds.h << std::endl;

      // Checking if the display is the primary one.
      // The check isn't pretty reliable, but this is all that SDL gives us...
      if (num_modes == 1 && modes[0]->w == 720 && modes[0]->h == 1280) {
        // We only support vertical orientation for the main screen (that is by default portrait)
        display_info.orientation = DISPLAY_ORIENTATION_LEFT_VERTICAL;
        res.insert(res.begin(), std::make_pair(instance_id, display_info));
      } else {
        display_info.orientation = DISPLAY_ORIENTATION_HORIZONTAL;
        // Pushing all the other displays to the end of the list
        res.push_back(std::make_pair(instance_id, display_info));
      }
    }
  }
  return res;
}

void SDLGraphAppBase::initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    throwSDLError("Couldn't initialize SDL!");
  }
}

IntSize SDLGraphAppBase::getScreenSize() {
  //auto main_display_id = getMainDisplay();
  //if (main_display_id == nullptr) {
  //  throw std::runtime_error("Can not find the standard display");
  //}

  //SDL_Rect display_bounds;
  //SDL_GetDisplayBounds( *main_display_id, &display_bounds );


  // int count;
  // const SDL_DisplayID* ids = SDL_GetDisplays(&count);
  // if (ids == nullptr) throwSDLError("Can not get the displays info.");
  // if (count < 1) {
  //   throw runtime_error("No displays found");
  // }

  // const SDL_DisplayMode* dm = SDL_GetCurrentDisplayMode(ids[0]);
  // if (dm == 0) throwSDLError("Can not get screen size.");

  DisplayOrientation orientation = getCurrentDisplayInfo().orientation;
  auto display_size = getCurrentDisplayInfo().size;
  if (orientation == DISPLAY_ORIENTATION_LEFT_VERTICAL) {
    return IntSize { display_size.height, display_size.width };
  } else {
    return IntSize { display_size.width, display_size.height };
  }
}

SDLGraphAppBase::WinCon SDLGraphAppBase::createSDLWindowAndContext(SDL_DisplayID displayId) {

  //std::vector<std::pair<SDL_DisplayID, DisplayInfo>> displays = getDisplays();

  // if (main_display_id == nullptr) {
  //   throw std::runtime_error("Can not find the standard display");
  // }

  SDL_Rect display_bounds;
  SDL_GetDisplayBounds( displayId, &display_bounds );
  printf("sdl win cr: %d, %d, %d, %d\n", display_bounds.x, display_bounds.y, display_bounds.w, display_bounds.h);

  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetStringProperty(props, "title", "SDL window");
  SDL_SetNumberProperty(props, "x", display_bounds.x);
  SDL_SetNumberProperty(props, "y", display_bounds.y);
  SDL_SetNumberProperty(props, "width", display_bounds.w);
  SDL_SetNumberProperty(props, "height", display_bounds.h);
  SDL_SetNumberProperty(props, "flags", SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN*/);
  SDL_Window* window = SDL_CreateWindowWithProperties(props);
  SDL_DestroyProperties(props);

  if (window == nullptr) {
    throwSDLError("Can't create a window.");
  }

  // try and setup a GL context
  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    throwSDLError("Can't create context.");
  }

  return { window, glContext };
}

void SDLGraphAppBase::makeGLContextCurrent(const WinCon& winCon) {

  int success =  SDL_GL_MakeCurrent(winCon.window, winCon.context);
  if (success != 0) {
    throwSDLError("Can't make the context current.");
  }

  //uint32_t windowFormat = SDL_GetWindowPixelFormat(window);
  //int contextType;
  //SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);
}

// void SDLGraphAppBase::createSkiaContext() {
//   auto interface = GrGLInterfaces::MakeEGL();
//   sContext = GrDirectContexts::MakeGL(interface);
// }

std::pair<sk_sp<GrDirectContext>, sk_sp<SkSurface>> SDLGraphAppBase::createSkiaSurface(int w, int h) {
  auto interface = GrGLInterfaces::MakeEGL();
  sk_sp<GrDirectContext> sContext = GrDirectContexts::MakeGL(interface);

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
  sk_sp<SkSurface> sSurface = SkSurfaces::WrapBackendRenderTarget(recContext,
      backendRenderTarget,
      kBottomLeft_GrSurfaceOrigin,
      colorType,
      nullptr,
      nullptr);

  if (sSurface == nullptr) {
    throw runtime_error("Can not create Skia surface.");
  }
  return { sContext, sSurface };
}

SDLGraphAppBase::SDLGraphAppBase() {
  createFontMgr();
  initSDL();

  std::vector<std::pair<SDL_DisplayID, DisplayInfo>> displays = getDisplays();

  for (size_t index = 0; index < displays.size(); index++) {
    auto id = displays[index].first;
    WinCon winCon = createSDLWindowAndContext(id);
    windowsAndContexts.push_back(winCon);

    makeGLContextCurrent(winCon);
    //if (index == 0) createSkiaContext();  // !!!!!!! TODO Make itt correct way
    SDL_DisplayID displayId = displays[index].first;
    DisplayInfo& displayInfo = displays[index].second;

//    if (displays[index].second.orientation == DISPLAY_ORIENTATION_LEFT_VERTICAL) {
//      sSurfaces.push_back({ displayId, createSkiaSurface(sContext, displayInfo.size.height, displayInfo.size.width) });
//    } else {
      auto conSurf = createSkiaSurface(/*sContext,*/ displayInfo.size.width, displayInfo.size.height);
      sSurfaces.push_back({ displayId, conSurf.first, conSurf.second });
//    }
  }
}

SDLGraphAppBase::~SDLGraphAppBase() {
  //sSurface = nullptr;
  //sContext = nullptr;
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
  SkCanvas* canvas = get<2>(sSurfaces[getCurrentDisplayIndex()])->getCanvas();
  if (canvas == nullptr) {
    throw runtime_error(string("Can not get a canvas for the surface"));
  }
  return canvas;
}

void SDLGraphAppBase::commitDrawing() {
  auto skiaContext = std::get<1>(sSurfaces[getCurrentDisplayIndex()]);
  skiaContext->flushAndSubmit();
  SDL_GL_SwapWindow(windowsAndContexts[getCurrentDisplayIndex()].window);
}

