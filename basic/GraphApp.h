#pragma once

#include "SDLGraphAppBase.h"

class GraphApp : public SDLGraphAppBase {

private:
  SkColor4f backColor;
  SkColor4f foreColor;
  std::string typefaceName;
  float fontSize;

  SkPaint backPaint;
  SkPaint forePaint;
  sk_sp<SkTypeface> typeface;
  SkFont font;

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
  GraphApp()
      : backColor(SkColor4f({ 0.2f, 0.2f, 0.2f, 1.0f} )),
        foreColor(SkColor4f({ 0.8f, 0.8f, 0.8f, 1.0f })),
        typefaceName("sans-serif"),
        fontSize(20.0f),
        typeface(getTypeface(typefaceName)) {
    updateFont();
    updateForePaint();
    updateBackPaint();

    onInit();
  }

  ~GraphApp() {
    onFin();
  }

  void setFontName(const std::string name) {
    typefaceName = name;
    updateFont();
  }

  void setFontSize(float size) {
    fontSize = size;
    updateFont();
  }

  // "Global" drawing fuunctions go here
  void setBackColor(float r, float g, float b, float a = 1.0f) {
    backColor.fA = 1.0;
    backColor.fR = r;
    backColor.fG = g;
    backColor.fB = b;
    updateBackPaint();
  }

  void setForeColor(float r, float g, float b, float a = 1.0f) {
    foreColor.fA = 1.0;
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

  void clear() {
    getCanvas()->clear(backColor);
  }

  // Should be implemented by the user
  virtual void onInit();
  virtual void onLoop();
  virtual void onFin();
/*
    SkPaint paint, paint2;
    auto tf = app.getTypeface("sans-serif");

    auto* canvas = app.getCanvas();
    //canvas->scale((float)0.5, (float)0.5);
    canvas->clear(SK_ColorBLUE);

    paint2.setColor(SK_ColorRED);
    canvas->drawRect({100, 200, 400, 300}, paint2);

    SkFont font(tf, 32);
    paint.setColor(SK_ColorWHITE);
    canvas->drawString("This is a string!!! It is printed, really!!!!", 50.0f, 30.0f, font, paint);
*/


};

