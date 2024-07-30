#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std;
using clk = chrono::high_resolution_clock;

clk::time_point start;

onInit {
  start = clk::now();
}

onLoop {
  auto scrSize = getScreenSize();
  auto l = scrSize.w / 2, t = scrSize.h / 2;

  auto w = 600, h = 350;
  l -= w / 2;
  t -= h / 2;

  setFPS(10);

  auto curTime = clk::now();
  double secd = chrono::duration_cast<chrono::milliseconds>(curTime - start).count();
  secd /= 1000;

  int sec = secd;

  int min = sec / 60;
  sec = (int)sec % 60;

  int hr = min / 60;
  min = min % 60;

  secd = secd - (int)(secd / 60) * 60;

  setBackColor(0.25, 0.25, 0.5);
  clear();
  setBackColor(0.75, 0.0, 0.0);
  drawRect(l + 90, t + 60, l + 290, t + 260);

  stringstream hrmin_ss;
  hrmin_ss << setfill('0') << setw(2) << hr
           << ":"
           << setfill('0') << setw(2) << min;

  stringstream sec_ss;
  sec_ss << ":" << fixed << setfill('0') << setw(4) << setprecision(1) << secd;

  setFontSize(80.0f);
  drawString(hrmin_ss.str(), l + 100, t + 185.0f);
  setFontSize(60.0f);
  drawString(sec_ss.str(), l + 335, t + 184.0f);
}

onFin { }
