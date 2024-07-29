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
  auto curTime = clk::now();
  double sec = chrono::duration_cast<chrono::milliseconds>(curTime - start).count();
  sec /= 1000;

  setBackColor(0.25, 0.25, 0.5);
  clear();
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << sec;

  setFontSize(80.0f);
  drawString(ss.str(), 10, 90.0f);
}

onFin { }
