#include "compilecontroller.h"
#include "application.h"
#include "guiloader.h"
#include "language.h"
#include "log.h"
#include "qtiny/qtiny.h"
#include "stringutils.hpp"
#include <map>

CompileController::CompileController(void* parent, const std::string& appPath) {
  // Store main controller to restore it later
  oldController = Application::Instance()->GetController();

  // Generate gui
  std::map<std::string, void*> widgets = GuiLoader::Load(appPath + "cfg/compile.xml", parent);
  win = widgets["win"];
  editor = widgets["editor"];
  button = widgets["button"];

  // Show window
  qShowWidget(win, TRUE);

  // No active program
  program = NULL;
}

CompileController::~CompileController() {
  if ( button ) qFreeWidget(button);
  if ( editor ) qFreeWidget(editor);
  if ( win ) qFreeWidget(win);
  if ( program ) qFreeProgram(program);
}

void CompileController::Run(const std::vector<std::string> &commands, const std::string& filename, const std::string& appPath) {
  // Store commands list and filename
  this->commands = commands;
  this->filename = filename;
  this->appPath = appPath;

  // Run first command
  currentCommand = -1;
  RunNextCommand();
}

void CompileController::WindowClosed(void *win) {
  if ( win == this->win ) {
    Application::Instance()->SetController(oldController, true);
  }
}

void CompileController::WidgetPerformedAction(void *widget, int id) {
  if ( widget == button ) {
    if ( program )
      qTerminateProgram(program);
    else
      WindowClosed(win);
  }
}

void CompileController::ProgramError(void *program, bool crash) {
  if ( program == this->program) {
    qSetWidgetStyleSheet(editor, "color: red");
    if ( crash ) {
      qSetEditorText(editor, "Program crashed.");
    } else {
      qSetEditorText(editor, "Could not run program.");
    }
    this->program = NULL;
    qSetButtonText(button, "Ok");
  }
}

void CompileController::ProgramFinished(void *program) {
  if ( program == this->program ) {
    // Free the program
    qFreeProgram(program);
    this->program = NULL;

    // Run next command
    bool wasNext = RunNextCommand();

    // If there wasn't another command, enable "Ok" button
    if ( !wasNext )
      qSetButtonText(button, "Ok");
  }
}

void CompileController::ProgramOutputAvailable(void *program) {
  if ( program == this->program ) {
    if ( std::string(qProgramErrorOutput(program)) != "" ) {
      qSetWidgetStyleSheet(editor, "color: red");
      qSetEditorText(editor, qProgramErrorOutput(program));
    } else {
      qSetEditorText(editor, stringutils::replace(std::string(qEditorText(editor)) + qProgramStandardOutput(program), "\r\n", "\n").c_str());
    }
  }
}

bool CompileController::RunNextCommand() {
  currentCommand++;

  if ( currentCommand < (int)commands.size() ) {
    // Fix command line
    std::string cmdline = stringutils::replace(commands[currentCommand], "%SEF", stringutils::stripExt(filename));
    cmdline = stringutils::replace(cmdline, "%F", filename);
    cmdline = stringutils::replace(cmdline, "%P", appPath);

    // Get dir
    std::string path = stringutils::extractPath(filename);

    // Launch program
    Log::Write("Running command line '" + cmdline + "'...");
    program = qRunProgram(cmdline.c_str(), path.c_str(), FALSE);

    return true;
  } else {
    Log::Write("No more commands to run");
    return false;
  }
}
