#pragma once

#include "basic_api.h"

#include "externals.h"

#include <SDL3/SDL.h>

#include <tuple>
#include <vector>

class SDLGraphAppBase {
private:
  struct WinCon {
    SDL_Window *window;
    SDL_GLContext context;
  };

  static void throwSDLError(const std::string& msg);
  static WinCon createSDLWindowAndContext(SDL_DisplayID displayId);
  static std::pair<sk_sp<GrDirectContext>, sk_sp<SkSurface>> createSkiaSurface(int w, int h);

  //sk_sp<GrDirectContext> sContext = nullptr;
  std::vector<std::tuple<SDL_DisplayID, sk_sp<GrDirectContext>, sk_sp<SkSurface>>> sSurfaces;
  sk_sp<SkFontMgr> fontMgr;

  uint8_t currentDisplayIndex;

  void createFontMgr();
  void initSDL();

  //void createSkiaContext();

public:
  static void makeGLContextCurrent(const WinCon& winCon);
  std::vector<WinCon> windowsAndContexts;

  SDLGraphAppBase();
  virtual ~SDLGraphAppBase();

  sk_sp<SkTypeface> getTypeface(const std::string& name);
  SkCanvas* getCanvas();
  void commitDrawing();

  // This function guarantees that the main display will be the first (index=0) one
  static std::vector<std::pair<SDL_DisplayID, DisplayInfo>> getDisplays();

  DisplayInfo getCurrentDisplayInfo() {
    std::vector<std::pair<SDL_DisplayID, DisplayInfo>> displays = getDisplays();
    return displays[currentDisplayIndex].second;
  }
  void setCurrentDisplayIndex(uint8_t index) { currentDisplayIndex = index; }
  uint8_t getCurrentDisplayIndex() { return currentDisplayIndex; }


  IntSize getScreenSize();
};
