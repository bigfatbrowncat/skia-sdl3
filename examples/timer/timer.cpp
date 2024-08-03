#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>

#include <GraphApp.h>

using namespace std;
using clk = chrono::high_resolution_clock;

class UserApp : public GraphApp {

  clk::time_point start;
  float angle;

  ofstream power_log;
  int prev_min;

public:
  UserApp() {
    start = clk::now();
    angle = 0.0;
    power_log = ofstream("powerlog.txt");
  }

  void onLoop() {
    ifstream bat("/sys/class/power_supply/axp20x-battery/capacity");
    string capacity;
    bat >> capacity;

    auto scrSize = getScreenSize();
    auto l = scrSize.w / 2, t = scrSize.h / 2;

    auto w = 600, h = 350;
    l -= w / 2;
    t -= h / 2;

    setFPS(20);

    auto curTime = clk::now();
    double secd = chrono::duration_cast<chrono::milliseconds>(curTime - start).count();
    secd /= 1000;

    angle = secd / 60 * 360;

    int sec = secd;

    int min = sec / 60;
    sec = (int)sec % 60;

    int cur_min = min;

    int hr = min / 60;
    min = min % 60;

    secd = secd - (int)(secd / 60) * 60;

    setBackColor(0.25, 0.25, 0.5);
    clear();
    setBackColor(0.75, 0.0, 0.0);

    translate(l + 190, t + 160);
    rotate(angle);

    drawRect(-100, -100, 100, 100);

    rotate(-angle);
    translate(-l - 190, -t - 160);

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

    setFontSize(30.0f);
    drawString(capacity, 20, 40);

    if (cur_min != prev_min) {
      prev_min = cur_min;
      power_log << hrmin_ss.str() << sec_ss.str() << "\t" << capacity << endl;
    }

    int capacity_int = stoi(capacity);
    if (capacity_int < 5) {
      system("poweroff");
    }
  }

};

BASIC_ENTRY(UserApp)
