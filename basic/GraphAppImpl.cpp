#include "GraphAppImpl.h"

void GraphAppImpl::updateFont() {
  typeface = getTypeface(typefaceName);
  font = SkFont(typeface, fontSize);
}

void GraphAppImpl::updateForePaint() {
  forePaint = SkPaint(foreColor);
}

void GraphAppImpl::updateBackPaint() {
  backPaint = SkPaint(backColor);
}

GraphAppImpl::GraphAppImpl(GraphAppCallbacks* cb)
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

GraphAppImpl::~GraphAppImpl() {
}

void GraphAppImpl::setFontName(const std::string name) {
  typefaceName = name;
  updateFont();
}

void GraphAppImpl::setFontSize(float size) {
  fontSize = size;
  updateFont();
}

void GraphAppImpl::setFPS(int FPS) {
  this->FPS = FPS;
}

int GraphAppImpl::getFPS() {
  return FPS;
}

// "Global" drawing fuunctions go here
void GraphAppImpl::setBackColor(float r, float g, float b, float a) {
  backColor.fA = a;
  backColor.fR = r;
  backColor.fG = g;
  backColor.fB = b;
  updateBackPaint();
}

void GraphAppImpl::setForeColor(float r, float g, float b, float a) {
  foreColor.fA = a;
  foreColor.fR = r;
  foreColor.fG = g;
  foreColor.fB = b;
  updateForePaint();
}

void GraphAppImpl::drawRect(float x1, float y1, float x2, float y2) {
  getCanvas()->drawRect({ x1, y1, x2, y2 }, backPaint);
}

void GraphAppImpl::drawString(const std::string& str, float x, float y) {
  getCanvas()->drawString(SkString(str), x, y, font, forePaint);
}

void GraphAppImpl::rotate(float angle_deg) {
  getCanvas()->rotate(angle_deg);
}

void GraphAppImpl::translate(float dx, float dy) {
  getCanvas()->translate(dx, dy);
}

void GraphAppImpl::clear() {
  getCanvas()->clear(backColor);
}
