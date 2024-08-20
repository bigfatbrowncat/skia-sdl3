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
    sk_sp<GrDirectContext> sContext;
  };

  static void throwSDLError(const std::string& msg);
  WinCon createSDLWindowAndContext(SDL_DisplayID displayId);
  sk_sp<SkSurface> createSkiaSurface(WinCon& wincon, int w, int h);

  //sk_sp<GrDirectContext> sContext = nullptr;
  std::vector<std::pair<SDL_DisplayID, sk_sp<SkSurface>>> sSurfaces;
  std::vector<std::pair<SDL_DisplayID, DisplayInfo>> displays;
  sk_sp<SkFontMgr> fontMgr;

  uint8_t currentDisplayIndex;

  void createFontMgr();
  void initSDL();

  int FPS;
  int newFPS;
  //sk_sp<GrDirectContext> sContext;

public:
  static void makeGLContextCurrent(const WinCon& winCon);
  std::vector<WinCon> windowsAndContexts;

  SDLGraphAppBase();
  virtual ~SDLGraphAppBase();

  sk_sp<SkTypeface> getTypeface(const std::string& name);
  SkCanvas* getCanvas();
  void commitDrawing();

  // This function guarantees that the main display will be the first (index=0) one
  void updateDisplays();
  const std::vector<std::pair<SDL_DisplayID, DisplayInfo>>& getDisplays() { return displays; }

  SDL_DisplayID getCurrentDisplayId() {
    const std::vector<std::pair<SDL_DisplayID, DisplayInfo>>& displays = getDisplays();
    return displays[currentDisplayIndex].first;
  }

  DisplayInfo getCurrentDisplayInfo() {
    const std::vector<std::pair<SDL_DisplayID, DisplayInfo>>& displays = getDisplays();
    return displays[currentDisplayIndex].second;
  }
  void setCurrentDisplayIndex(uint8_t index) { currentDisplayIndex = index; }
  uint8_t getCurrentDisplayIndex() { return currentDisplayIndex; }

  void setFPS(int FPS);
  int getFPS();
  void updateSurfacesIfFPSUpdated();

  IntSize getScreenSize();
};
