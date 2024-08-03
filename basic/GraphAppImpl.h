#pragma once

#include "SDLGraphAppBase.h"

struct GraphAppCont;

typedef void init_callback(GraphAppCont*);
typedef void loop_callback(GraphAppCont*);
typedef void fin_callback(GraphAppCont*);

struct GraphAppCallbacks {
  init_callback* onInit;
  loop_callback* onLoop;
  fin_callback* onFin;
};

class GraphAppImpl : public SDLGraphAppBase {

private:
  SkColor4f backColor;
  SkColor4f foreColor;
  std::string typefaceName;
  float fontSize;

  SkPaint backPaint;
  SkPaint forePaint;
  sk_sp<SkTypeface> typeface;
  SkFont font;

  int FPS;

  void updateFont();
  void updateForePaint();
  void updateBackPaint();

public:
  GraphAppCallbacks* cb;

  GraphAppImpl(GraphAppCallbacks* cb);
  ~GraphAppImpl();

  void setFontName(const std::string name);
  void setFontSize(float size);
  void setFPS(int FPS);
  int getFPS();

  // "Global" drawing fuunctions go here
  void setBackColor(float r, float g, float b, float a = 1.0f);
  void setForeColor(float r, float g, float b, float a = 1.0f);
  void drawRect(float x1, float y1, float x2, float y2);
  void drawString(const std::string& str, float x, float y);
  void rotate(float angle_deg);
  void translate(float dx, float dy);
  void clear();

};

struct GraphAppCont {
  GraphAppImpl* impl;
};
