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

  void updateFont() {
    typeface = getTypeface(typefaceName);
    font = SkFont(typeface, fontSize);
  }

  void updateForePaint() {
    forePaint = SkPaint(foreColor);
  }

  void updateBackPaint() {
    backPaint = SkPaint(backColor);
  }

public:
  GraphAppCallbacks* cb;

  GraphAppImpl(GraphAppCallbacks* cb)
      : cb(cb),
        backColor(
          SkColor4f({ 0.2f, 0.2f, 0.2f, 1.0f })
        ),
        foreColor(
          SkColor4f({ 0.8f, 0.8f, 0.8f, 1.0f })
        ),
        typefaceName("sans-serif"),
        fontSize(20.0f),
        typeface(getTypeface(typefaceName)),
        FPS(60) {
    updateFont();
    updateForePaint();
    updateBackPaint();
  }

  ~GraphAppImpl() {
  }

  void setFontName(const std::string name) {
    typefaceName = name;
    updateFont();
  }

  void setFontSize(float size) {
    fontSize = size;
    updateFont();
  }

  void setFPS(int FPS) {
    this->FPS = FPS;
  }

  int getFPS() {
    return FPS;
  }

  // "Global" drawing fuunctions go here
  void setBackColor(float r, float g, float b, float a = 1.0f) {
    backColor.fA = a;
    backColor.fR = r;
    backColor.fG = g;
    backColor.fB = b;
    updateBackPaint();
  }

  void setForeColor(float r, float g, float b, float a = 1.0f) {
    foreColor.fA = a;
    foreColor.fR = r;
    foreColor.fG = g;
    foreColor.fB = b;
    updateForePaint();
  }

  void drawRect(float x1, float y1, float x2, float y2) {
    getCanvas()->drawRect({ x1, y1, x2, y2 }, backPaint);
  }

  void drawString(const std::string& str, float x, float y) {
    getCanvas()->drawString(SkString(str), x, y, font, forePaint);
  }

  void rotate(float angle_deg) {
    getCanvas()->rotate(angle_deg);
  }

  void translate(float dx, float dy) {
    getCanvas()->translate(dx, dy);
  }

  void clear() {
    getCanvas()->clear(backColor);
  }

  // Should be implemented by the user
  //virtual void onInit();
  //virtual void onLoop();
  //virtual void onFin();

};

struct GraphAppCont {
  GraphAppImpl* impl;
};
