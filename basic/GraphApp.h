#pragma once

#include "basic_api.h"

#include <string>
#include <map>

struct GraphAppCont;

class GraphApp;

typedef GraphApp* GraphAppFactory();

class GraphApp {
private:
  GraphAppCont* cont;

  static std::map<GraphAppCont*, GraphApp*> apps;

public:
  static GraphAppFactory* factory;
  static void staticInit(GraphAppCont* cont) {
    GraphApp* newApp = GraphApp::factory();
    apps.insert({
      cont,
      newApp
    });
    newApp->cont = cont;
  }
  static void staticLoop(GraphAppCont* cont) {
    GraphApp* app = apps[cont];
    app->onLoop();
  }
  static void staticFin(GraphAppCont* cont) {
    delete apps[cont];
    apps.erase(cont);
  }

  GraphApp() { }

  void setFontName(const std::string name) {
    GraphApp_setFontName(cont, name.c_str());
  }

  void setFontSize(float size) {
    GraphApp_setFontSize(cont, size);
  }

  void setFPS(int fps) {
    GraphApp_setFPS(cont, fps);
  }

  int getFPS() {
    return GraphApp_getFPS(cont);
  }

  // "Global" drawing fuunctions go here
  void setBackColor(float r, float g, float b, float a = 1.0f) {
    GraphApp_setBackColor(cont, r, g, b, a);
  }

  void setForeColor(float r, float g, float b, float a = 1.0f) {
    GraphApp_setForeColor(cont, r, g, b, a);
  }

  void drawRect(float x1, float y1, float x2, float y2) {
    GraphApp_drawRect(cont, x1, y1, x2, y2);
  }

  void drawString(const std::string& str, float x, float y) {
    GraphApp_drawString(cont, str.c_str(), x, y);
  }

  void rotate(float angle_deg) {
    GraphApp_rotate(cont, angle_deg);
  }

  void translate(float dx, float dy) {
    GraphApp_translate(cont, dx, dy);
  }

  void clear() {
    GraphApp_clear(cont);
  }

  IntSize getScreenSize() {
    return GraphApp_getScreenSize(cont);
  }

  TextMeasures measureText(const std::string& text) {
    return GraphApp_measureText(cont, text.c_str());
  }

  virtual void onLoop() = 0;
};

#define BASIC_ENTRY(USER_APP)					\
        std::map<GraphAppCont*, GraphApp*> GraphApp::apps;	\
        int main() {						\
          GraphAppCallbacks cb;					\
          cb.onInit = &GraphApp::staticInit;			\
          cb.onLoop = &GraphApp::staticLoop;			\
          cb.onFin = &GraphApp::staticFin;			\
								\
          GraphApp_main(&cb);					\
								\
          return 0;						\
        }							\
								\
        GraphAppFactory* GraphApp::factory =			\
            []() -> GraphApp* { return new USER_APP; };
