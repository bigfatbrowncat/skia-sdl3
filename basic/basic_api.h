#pragma once

struct GraphAppCont;
struct GraphAppCallbacks;

typedef void init_callback(GraphAppCont*);
typedef void loop_callback(GraphAppCont*);
typedef void fin_callback(GraphAppCont*);

struct GraphAppCallbacks {
  init_callback* onInit;
  loop_callback* onLoop;
  fin_callback* onFin;
};

typedef struct IntSize {
  int w,h;
} IntSize;

typedef struct TextBounds {
  float left, top, right, bottom;
} TextBounds;

typedef struct TextMeasures {
  TextBounds bounds;
  float width;
} TextMeasures;


extern "C" {

int GraphApp_main(GraphAppCallbacks* cb);

void GraphApp_setFontName(GraphAppCont* self, const char* name);
void GraphApp_setFontSize(GraphAppCont* self, float size);
void GraphApp_setFPS(GraphAppCont* self, int FPS);
int GraphApp_getFPS(GraphAppCont* self);
void GraphApp_setBackColor(GraphAppCont* self, float r, float g, float b, float a);
void GraphApp_setForeColor(GraphAppCont* self, float r, float g, float b, float a);
void GraphApp_drawRect(GraphAppCont* self, float x1, float y1, float x2, float y2);
void GraphApp_drawString(GraphAppCont* self, const char* str, float x, float y);
void GraphApp_rotate(GraphAppCont* self, float angle_deg);
void GraphApp_translate(GraphAppCont* self, float dx, float dy);
void GraphApp_clear(GraphAppCont* self);
IntSize GraphApp_getScreenSize(GraphAppCont* self);
TextMeasures GraphApp_measureText(GraphAppCont* self, const char* text);
}
