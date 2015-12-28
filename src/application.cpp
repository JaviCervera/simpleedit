#include "application.h"
#include "config.h"
#include "language.h"
#include "log.h"
#include "maincontroller.h"
#include "qtiny/qtiny.h"
#include "stringutils.hpp"
#include <string>

using namespace std;

Application* Application::instance = NULL;

Application* Application::Instance() {
  if ( !instance )
    instance = new Application();
  return instance;
}

void Application::SetController(Controller *controller, bool deleteOld) {
  if ( controller != this->controller ) {
    if ( deleteOld )
      delete this->controller;
    this->controller = controller;
  }
}

int Application::Run(int argc, char **argv) {
  int ret = 0;

  // Init Qtiny
  qAppInit(argc, argv);

  // Init log
  Log::Init();
  Log::Write("Log initialized");

  // Base filename and path
  string appPath = stringutils::extractPath(argv[0]);
  string baseFile = stringutils::stripPath(stringutils::stripExt(argv[0]));

  // Parse config
  string configFile = appPath + "cfg/" + baseFile + ".config.xml";
  Log::Write("Parsing config file '" + configFile + "'...");
  config = new Config(configFile);
  Log::Write("Config parsed");

  // Parse language
  string languageFile = appPath + "cfg/" + baseFile + ".language.xml";
  Log::Write("Parsing language file '" + languageFile + "'...");
  language = new Language(languageFile);
  Log::Write("Language file parsed");

  // Register font
  config->RegisterFont();

  // Set MainController as current controller
  MainController* controller = new MainController(argc, argv);
  SetController(controller, false);

  // Run application
  if ( ((MainController*)controller)->IsInitialized() )
    ret = qAppRun(EventHandler);
  else
    qNotify(NULL, "Error", "Interface could not be generated.\nApplication will now close.", QNOTIFY_ERROR);

  // Cleanup
  Log::Write("Exiting application...");
  SetController(NULL, true);
  Log::Write("Cleaning up Qtiny...");
  qAppCleanup();
  Log::Write("Qtiny cleaned up");
  delete config;
  delete language;

  return ret;
}

void Application::EventHandler(int event, void *widget, int id) {
  switch ( event ) {
  case QEVENT_MENUACTION:
    Application::Instance()->GetController()->MenuIdSelected(id);
    break;
  case QEVENT_WIDGETACTION:
    Application::Instance()->GetController()->WidgetPerformedAction(widget, id);
    break;
  case QEVENT_WINDOWCLOSE:
    Application::Instance()->GetController()->WindowClosed(widget);
    break;
  case QEVENT_WINDOWSIZE:
    Application::Instance()->GetController()->WindowResized(widget);
    break;
  case QEVENT_PROGRAMFINISH:
    Application::Instance()->GetController()->ProgramFinished(widget);
    break;
  case QEVENT_PROGRAMOUTPUT:
    Application::Instance()->GetController()->ProgramOutputAvailable(widget);
    break;
  case QEVENT_PROGRAMERROR:
    Application::Instance()->GetController()->ProgramError(widget, id);
    break;
  }
}
