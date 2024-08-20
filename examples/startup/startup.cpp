#include <string>
#include <iostream>
using namespace std;

INIT() {
}

FIN() {
}

int ff = 0;

LOOP() {
  // Setting minimal FPS
  if (ff++ < 2) { setFPS(ff+1); cout << "----------------" << ff << flush << endl; }

  // Finding out the screen size
  auto scrSize = getScreenSize();

  // Clearing the background with blue
  setBackColor(0.25, 0.25, 0.5);
  clear();

  string hello = "Hello, world! " + to_string(scrSize.width) + "x" + to_string(scrSize.height);

  // Setting the font size proportional to the screen size
  setFontSize(scrSize.width / 16.0);

  // Measuring the text line using the current font
  auto textMeasures  = measureText(hello);

  // Calculating the text width and height
  auto w = textMeasures.width, \
       h = textMeasures.bounds.bottom - textMeasures.bounds.top;

  // Setting l and t to the center of the screen
  auto l = scrSize.width / 2, t = scrSize.height / 2;

  // Moving l left half of the text size
  l -= w / 2;

  // Moving t down half of the text size (there should be the baseline)
  t += h / 2;

  // Drawing the text starting at the l and t
  drawString(hello, l, t);
}
