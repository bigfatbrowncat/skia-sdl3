using namespace std;

INIT() {
}

FIN() {
}

LOOP() {
  // Setting minimal FPS
  setFPS(1);

  // Finding out the screen size
  auto scrSize = getScreenSize();

  // Clearing the background with blue
  setBackColor(0.25, 0.25, 0.5);
  clear();

  string hello = "Hello, world!";

  // Setting the font size proportional to the screen size
  setFontSize(scrSize.w / 8);

  // Measuring the text line using the current font
  auto textMeasures = measureText(hello);

  // Calculating the text width and height
  auto w = textMeasures.width, \
       h = textMeasures.bounds.bottom - textMeasures.bounds.top;

  // Setting l and t to the center of the screen
  auto l = scrSize.w / 2, t = scrSize.h / 2;

  // Moving l left half of the text size
  l -= w / 2;

  // Moving t down half of the text size (there should be the baseline)
  t += h / 2;

  // Drawing the text starting at the l and t
  drawString(hello, l, t);
}
