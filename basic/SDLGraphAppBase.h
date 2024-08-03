#pragma once

#include "basic_api.h"

#include "externals.h"

#include <SDL3/SDL.h>

class SDLGraphAppBase {
private:
  sk_sp<GrDirectContext> sContext = nullptr;
  sk_sp<SkSurface> sSurface = nullptr;
  sk_sp<SkFontMgr> fontMgr;

  SDL_GLContext glContext = nullptr;
  SDL_Window *window = NULL;

  void throwSDLError(const std::string& msg);
  void createFontMgr();
  void initSDL();

  void createSDLWindowAndContext();
  void makeGLContextCurrent(int w, int h);
  void createSkiaContext();
  void createSkiaSurface(int w, int h);

public:
  SDLGraphAppBase();
  virtual ~SDLGraphAppBase();

  sk_sp<SkTypeface> getTypeface(const std::string& name);
  SkCanvas* getCanvas();
  void commitDrawing();

  IntSize getScreenSize();
};
