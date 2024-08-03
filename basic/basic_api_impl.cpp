#include "basic_api.h"

#include "GraphAppImpl.h"

extern "C" {

void GraphApp_setFontName(GraphAppCont* self, const char* name) {
  self->impl->setFontName(std::string(name));
}

void GraphApp_setFontSize(GraphAppCont* self, float size) {
  self->impl->setFontSize(size);
}

void GraphApp_setFPS(GraphAppCont* self, int FPS) {
  self->impl->setFPS(FPS);
}

int GraphApp_getFPS(GraphAppCont* self) {
  return self->impl->getFPS();
}

void GraphApp_setBackColor(GraphAppCont* self, float r, float g, float b, float a) {
  self->impl->setBackColor(r, g, b, a);
}

void GraphApp_setForeColor(GraphAppCont* self, float r, float g, float b, float a) {
  self->impl->setForeColor(r, g, b, a);
}

void GraphApp_drawRect(GraphAppCont* self, float x1, float y1, float x2, float y2) {
  self->impl->drawRect(x1, y1, x2, y2);
}

void GraphApp_drawString(GraphAppCont* self, const char* str, float x, float y) {
  self->impl->drawString(std::string(str), x, y);
}

void GraphApp_rotate(GraphAppCont* self, float angle_deg) {
  self->impl->rotate(angle_deg);
}

void GraphApp_translate(GraphAppCont* self, float dx, float dy) {
  self->impl->translate(dx, dy);
}

void GraphApp_clear(GraphAppCont* self) {
  self->impl->clear();
}

IntSize GraphApp_getScreenSize(GraphAppCont* self) {
  return self->impl->getScreenSize();
}

FloatRect GraphApp_measureText(GraphAppCont* self, const char* text) {
  return self->impl->measureText(std::string(text));
}

}
