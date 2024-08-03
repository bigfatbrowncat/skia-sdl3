#include <GraphApp.h>

class UserApp : public GraphApp {
public:
  UserApp();
  ~UserApp();
  void onLoop();
};

BASIC_ENTRY(UserApp)

#define INIT  UserApp::UserApp
#define FIN   UserApp::~UserApp
#define LOOP  void UserApp::onLoop

