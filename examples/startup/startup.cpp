using namespace std;

INIT() {
}

FIN() {
}

LOOP() {
  setFPS(1);

  setBackColor(0.25, 0.25, 0.5);
  clear();

  string hello = "Hello, world!";

  setFontSize(150.0f);

  // Calculating the text position

  // Measuring the text line using the current font
  FloatRect textRect = measureText(hello);

  auto w = textRect.right - textRect.left, \
       h = textRect.bottom - textRect.top;

  // Finding out the screen size
  auto scrSize = getScreenSize();
  // Setting l and t to the center of the screen
  auto l = scrSize.w / 2, t = scrSize.h / 2;

  // Moving l left half of the text size
  l -= w / 2;
  // Moving t down half of the text size (there should be the baseline)
  t += h / 2;

  // Drawing
  drawString(hello, l, t);
}
