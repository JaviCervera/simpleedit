#include "maincontroller.h"
#include "application.h"
#include "compilecontroller.h"
#include "config.h"
#include "findcontroller.h"
#include "guiloader.h"
#include "language.h"
#include "log.h"
#include "qtiny/qtiny.h"
#include "stringutils.hpp"
#include "textarea.h"
#include "time.h"
#include <map>
#include <unistd.h>

int RunCommand(const std::string& command, const std::string& filename, const std::string& appPath);

MainController::MainController(int argc, char* argv[]) : functionCombo(NULL), buildTab(-1), functionUpdateLastMillisecs(0), jumpEnabled(true) {
  Log::Write("Creating MainController...");

  path = stringutils::extractPath(argv[0]);

  // Load Gui for this controller
  Log::Write("Parsing main interface file...");
  std::map<std::string, void*> guiMap = GuiLoader::Load(path + "cfg/mainwindow.xml", NULL);
  Log::Write("File parsed");
  win = guiMap["win"];
  menubar = guiMap["menu"];
  toolbar = guiMap["toolbar"];
  tabbar = guiMap["tabbar"];
  statusbar = guiMap["statusbar"];

  if ( IsInitialized() ) {
    // Set window title from config (ignore the one in the interface XML)
    qSetWindowTitle(win, Application::Instance()->GetConfig()->WinTitle().c_str());

    // Add function combo to toolbar
    /*functionCombo = qCreateComboBox(NULL);
        qAddComboBoxItem(functionCombo, "Functions:");
        qAddToolBarWidget(toolbar, functionCombo);*/

    // Add tab bar to layout engine
    AddWidget(tabbar);

    // Adjust interface dimensions
    WindowResized(win);

    // Add a new empty tab
    if ( argc > 1 ) {
      // Open files
      for ( int i = 1; i < argc; ++i ) DoOpen(argv[i]);
    } else {
      DoNew();
    }

    // Update status bar
    UpdateStatusBar();

    // Show window
    qShowWidget(win, TRUE);

    Log::Write("MainController created");
  } else {
    Log::Write("MainController interface could not be created");
  }
}

MainController::~MainController() {
  if ( win ) qFreeWidget(win);
}

bool MainController::IsInitialized() {
  return (win && menubar && toolbar && tabbar && statusbar);
}

void MainController::AddWidget(void *widget, int fixedHeight) {
  widgets.push_back(WidgetLayout(widget, fixedHeight));
  Layout();
}

void MainController::RemoveWidget(void *widget) {
  for ( std::vector<WidgetLayout>::iterator it = widgets.begin(); it != widgets.end(); it++ ) {
    if ( (*it).widget == widget ) {
      widgets.erase(it);
      break;
    }
  }
  Layout();
}

bool MainController::Find(const std::string &text, bool matchCase) {
  findText = text;
  this->matchCase = matchCase;
  return DoFindNext();
}

void MainController::WindowClosed(void* win) {
  if ( win == this->win )
    if ( DoCloseAll() )
      qAppExit();
}

void MainController::WindowResized(void* win) {
  if ( win == this->win ) Layout();
}

void MainController::MenuIdSelected(int id) {
  switch ( id ) {
  case 101:
    DoNew();
    break;
  case 102:
    DoOpen();
    break;
  case 103:
    DoSave();
    break;
  case 104:
    DoSaveAs();
    break;
  case 105:
    DoSaveAll();
    break;
  case 106:
    DoClose();
    break;
  case 107:
    DoCloseAll();
    break;
  case 108:
    DoNextFile();
    break;
  case 109:
    DoPrevFile();
    break;
  case 110:
    WindowClosed(win);
    break;
  case 201:
    qEditorUndo(SelectedTextArea()->GetWidget());
    break;
  case 202:
    qEditorRedo(SelectedTextArea()->GetWidget());
    break;
  case 203:
    qEditorCut(SelectedTextArea()->GetWidget());
    break;
  case 204:
    qEditorCopy(SelectedTextArea()->GetWidget());
    break;
  case 205:
    qEditorPaste(SelectedTextArea()->GetWidget());
    break;
  case 206:
    // Only open panel when active controller is this one
    if ( Application::Instance()->GetController() == this )
      Application::Instance()->SetController(new FindController(win, path), false);
    break;
  case 207:
    DoFindNext();
    break;
  case 208:
    // Options...
    break;
  case 301:
    // Run
    SelectBuildTextArea();
#ifdef USE_COMPILE_CONTROLLER
    Application::Instance()->SetController(new CompileController(win), false);
    ((CompileController*)Application::Instance()->GetController())->Run(Application::Instance()->GetLanguage()->RunCommands(), BuildTextArea()->GetFile(), path);
#else
    for ( std::vector<std::string>::const_iterator it = Application::Instance()->GetLanguage()->RunCommands().begin(); it != Application::Instance()->GetLanguage()->RunCommands().end(); ++it ) {
      RunCommand(*it, BuildTextArea()->GetFile(), path);
    }
#endif
    break;
  case 302:
    // Debug...
    //SelectBuildTextArea();
    break;
  case 303:
    // Build
    SelectBuildTextArea();
#ifdef USE_COMPILE_CONTROLLER
    Application::Instance()->SetController(new CompileController(win), false);
    ((CompileController*)Application::Instance()->GetController())->Run(Application::Instance()->GetLanguage()->BuildCommands(), BuildTextArea()->GetFile(), path);
#else
    for ( std::vector<std::string>::const_iterator it = Application::Instance()->GetLanguage()->BuildCommands().begin(); it != Application::Instance()->GetLanguage()->BuildCommands().end(); ++it ) {
      RunCommand(*it, BuildTextArea()->GetFile(), path);
    }
#endif
    break;
  case 304:
    // Compile
    SelectBuildTextArea();
#ifdef USE_COMPILE_CONTROLLER
    Application::Instance()->SetController(new CompileController(win), false);
    ((CompileController*)Application::Instance()->GetController())->Run(Application::Instance()->GetLanguage()->CompileCommands(), BuildTextArea()->GetFile(), path);
#else
    for ( std::vector<std::string>::const_iterator it = Application::Instance()->GetLanguage()->CompileCommands().begin(); it != Application::Instance()->GetLanguage()->CompileCommands().end(); ++it ) {
      RunCommand(*it, BuildTextArea()->GetFile(), path);
    }
#endif
    break;
  case 305:
    DoLockFile();
    break;
  case 401:
    qNotify(win, Application::Instance()->GetConfig()->WinTitle().c_str(), Application::Instance()->GetConfig()->About().c_str(), QNOTIFY_INFORMATION);
    break;
  }
  UpdateStatusBar();
}

void MainController::WidgetPerformedAction(void *widget, int id) {
  // If a text area changed, notify it
  for ( int i = 0; i < qCountTabBarItems(tabbar); i++ ) {
    if ( widget == qTabBarWidget(tabbar, i) && id == 0 ) {
      TextArea* textArea = (TextArea*)qWidgetProperty(qTabBarWidget(tabbar, i), "textarea");
      if ( textArea ) textArea->TextChanged();
      if ( functionCombo != NULL ) UpdateFunctionCombo();
    }
  }

  // Check function combo event
  if ( widget == functionCombo  &&  qComboBoxIndex(functionCombo) != 0  &&  jumpEnabled ) {
    TextArea* textArea = SelectedTextArea();
    if ( textArea != NULL ) {
      // Count characters to function line number
      int ofs = 0;
      std::vector<std::string> lines = stringutils::split(qEditorText(textArea->GetWidget()), '\n');
      for ( int i = 0; i < functionLines[qComboBoxIndex(functionCombo)-1]-1; i++ )
        ofs += lines[i].length() + 1;

      // Move cursor to line
      qSetEditorCursorPos(textArea->GetWidget(), ofs);
      qFocusWidget(textArea->GetWidget());
      qSetComboBoxIndex(functionCombo, 0);
    }
  }

  UpdateStatusBar();
}

void MainController::DoNew(std::string filename) {
  // Get tab name
  std::string title = "<untitled>";
  if ( filename != "" )
    title = stringutils::stripPath(filename);

  // Create text area
  TextArea* textarea = new TextArea(filename);

  // Add tab
  qAddTabBarItem(tabbar, textarea->GetWidget(), title.c_str());
  qSetTabBarIndex(tabbar, qCountTabBarItems(tabbar)-1);
  qFocusWidget(textarea->GetWidget());
}

void MainController::DoOpen(std::string filename) {
  if ( filename == "" ) {
    filename = qRequestFile(win, "Open", "", Application::Instance()->GetConfig()->FileFilter().c_str(), FALSE);
    Log::Write("File '" + filename + "' selected to open");
  }
  if ( filename != "" ) {
    TextArea* textArea = SelectedTextArea();
    DoNew(filename);
    SetStatus("Loaded '" + stringutils::stripPath(filename) + "'");
    if ( textArea != NULL  &&  textArea->GetFile() == ""  &&  !textArea->IsModified() ) {
      SelectTextArea(textArea);
      DoClose();
    }
  }
}

bool MainController::DoSave(bool checkModified) {
  TextArea* textArea = SelectedTextArea();
  if ( checkModified  &&  !textArea->IsModified() ) return false;
  if ( textArea->GetFile() == "" ) return DoSaveAs();
  textArea->Save();
  textArea->SetModified(false);
  qSetTabText(tabbar, qTabBarIndex(tabbar), stringutils::stripPath(textArea->GetFile()).c_str());
  SetStatus("Saved '" + textArea->GetFile() + "'");
  return true;
}

bool MainController::DoSaveAs() {
  TextArea* textArea = SelectedTextArea();
  Log::Write("Save as filename: " + textArea->GetFile());
  std::string file = qRequestFile(win, "Save As", textArea->GetFile().c_str(), Application::Instance()->GetConfig()->FileFilter().c_str(), TRUE);
  if ( file != "" ) {
    if ( stringutils::extractExt(file) == "" ) file += Application::Instance()->GetConfig()->DefaultExt();
    if ( stringutils::read(file) != "" ) {
      switch ( qConfirm(win, "Save As", "File already exists. Replace?", FALSE) ) {
      case QCONFIRM_YES:
        textArea->SetFile(file);
        return DoSave(FALSE);
      case QCONFIRM_NO:
        return DoSaveAs();
      }
    } else {
      textArea->SetFile(file);
      return DoSave(FALSE);
    }
  }
  return false;
}

void MainController::DoSaveAll() {
  int tab = qTabBarIndex(tabbar);
  for ( int i = 0; i < qCountTabBarItems(tabbar); i++ ) {
    qSetTabBarIndex(tabbar, i);
    DoSave();
  }
  qSetTabBarIndex(tabbar, tab);
}

bool MainController::DoClose() {
  TextArea* selTextArea = SelectedTextArea();

  // If modified, ask the user to save
  if ( selTextArea->IsModified() ) {
    switch ( qConfirm(win, Application::Instance()->GetConfig()->WinTitle().c_str(), "File has been modified!\rSave changes before closing?", TRUE) ) {
    case QCONFIRM_YES:
      if ( !DoSave() ) return false;
      break;
    case QCONFIRM_CANCEL:
      return false;
    }
  }

  // Remove text area
  delete (TextArea*)qWidgetProperty(qTabBarWidget(tabbar, qTabBarIndex(tabbar)), "textarea");
  qSetWidgetProperty(qTabBarWidget(tabbar, qTabBarIndex(tabbar)), "textarea", NULL);

  // Remove the tab
  qRemoveTabBarItem(tabbar, qTabBarIndex(tabbar));
  if ( qCountTabBarItems(tabbar) == 0 )
    DoNew();

  return true;
}

bool MainController::DoCloseAll() {
  int pending = qCountTabBarItems(tabbar);
  while ( pending > 0 ) {
    qSetTabBarIndex(tabbar, qCountTabBarItems(tabbar)-1);
    if ( !DoClose() ) return false;
    pending--;
  }
  return true;
}

void MainController::DoNextFile() {
  if ( qTabBarIndex(tabbar) < qCountTabBarItems(tabbar) - 1 )
    qSetTabBarIndex(tabbar, qTabBarIndex(tabbar)+1);
  else
    qSetTabBarIndex(tabbar, 0);
}

void MainController::DoPrevFile() {
  if ( qTabBarIndex(tabbar) > 0 )
    qSetTabBarIndex(tabbar, qTabBarIndex(tabbar)-1);
  else
    qSetTabBarIndex(tabbar, qCountTabBarItems(tabbar)-1);
}

bool MainController::DoFindNext(bool wrap) {
  bool ret = false;

  // Get text area to work with
  TextArea* textArea = SelectedTextArea();

  // If next occurence is found
  int pos = textArea->FindNext(findText, matchCase);
  if ( pos != -1 ) {
    //qSetEditorCursorPos(textArea->GetWidget(), pos);
    qSelectEditorText(textArea->GetWidget(), pos, findText.length());
    SetStatus("Find next");
    ret = true;
    // If not
  } else {
    if ( wrap ) {
      pos = textArea->FindNext(findText, matchCase, true);
      if ( pos != -1 ) {
        //qSetEditorCursorPos(textArea->GetWidget(), pos);
        qSelectEditorText(textArea->GetWidget(), pos, findText.length());
        SetStatus("End of file reached");
        ret = true;
      } else {
        SetStatus("No matches found");
      }
    } else {
      SetStatus("No matches found");
    }
  }

  UpdateStatusBar();

  return ret;
}

void MainController::DoLockFile() {
  if ( buildTab != qTabBarIndex(tabbar) )
    buildTab = qTabBarIndex(tabbar);
  else
    buildTab = -1;
}

void MainController::SelectTextArea(TextArea *textArea) {
  for ( int i = 0; i < qCountTabBarItems(tabbar); i++ ) {
    if ( (TextArea*)qWidgetProperty(qTabBarWidget(tabbar, i), "textarea") == textArea ) {
      qSetTabBarIndex(tabbar, i);
      break;
    }
  }
}

TextArea* MainController::SelectedTextArea() {
  int i = qTabBarIndex(tabbar);
  if ( i > -1 )
    return (TextArea*)qWidgetProperty(qTabBarWidget(tabbar, i), "textarea");
  else
    return NULL;
}

void MainController::SelectBuildTextArea() {
  for ( int i = 0; i < qCountTabBarItems(tabbar); i++ ) {
    if ( qWidgetProperty(qTabBarWidget(tabbar, i), "textarea") == BuildTextArea() ) {
      qSetTabBarIndex(tabbar, i);
      break;
    }
  }
}

TextArea* MainController::BuildTextArea() {
  if ( buildTab != -1 )
    return (TextArea*)qWidgetProperty(qTabBarWidget(tabbar, buildTab), "textarea");
  else
    return SelectedTextArea();
}

void MainController::SetStatus(std::string status) {
  statusText = status;
}

void MainController::UpdateStatusBar() {
  std::string txt = "";
  TextArea* textArea = SelectedTextArea();
  if ( textArea ) {
    int line = qEditorLine(textArea->GetWidget()) + 1;
    int col = qEditorColumn(textArea->GetWidget()) + 1;
    txt = "Line: " + stringutils::fromInt(line) + ", Col: " + stringutils::fromInt(col);
    if ( textArea->IsModified() ) txt += " *";
    if ( qTabBarIndex(tabbar) == buildTab ) txt += " (build)";
    txt.resize(40, ' ');
  }
  txt += statusText;
  qSetStatusBarText(statusbar, txt.c_str());
}

int MainController::ClientY() {
  int y = 0;
#ifndef __APPLE__
  if ( menubar ) y += qWidgetHeight(menubar);
#else
  y += 2;
#endif
  if ( toolbar ) y += qWidgetHeight(toolbar);
  return y;
}

int MainController::ClientHeight() {
  int height = qWidgetHeight(win) - ClientY();
  if ( statusbar ) height -= qWidgetHeight(statusbar);
  return height;
}

void MainController::Layout() {
  // Get number of free widgets and total fixed height
  int freeWidgets = 0;
  int totalFixedHeight = 0;
  for ( std::vector<WidgetLayout>::const_iterator it = widgets.begin(); it != widgets.end(); it++ ) {
    if ( (*it).fixedHeight == 0 )
      freeWidgets++;
    else
      totalFixedHeight += (*it).fixedHeight;
  }

  // Get height for each free widget
  int freeHeight = (ClientHeight() - totalFixedHeight) / freeWidgets;

  // Layout widgets
  int currentY = ClientY();
  for ( std::vector<WidgetLayout>::const_iterator it = widgets.begin(); it != widgets.end(); it++ ) {
    if ( (*it).fixedHeight == 0 ) {
      qLayoutWidget((*it).widget, 0, currentY, qWidgetWidth(win), freeHeight);
      currentY += freeHeight;
    } else {
      qLayoutWidget((*it).widget, 0, currentY, qWidgetWidth(win), (*it).fixedHeight);
      currentY += (*it).fixedHeight;
    }
  }
}

void MainController::UpdateFunctionCombo() {
  // Get select text area or return if there are none
  TextArea* textArea = SelectedTextArea();
  if ( textArea == NULL ) return;

  // Make sure that at least 5 seconds have elapsed
  if ( Millisecs() < functionUpdateLastMillisecs + 5000 ) return;

  // Get function list form the text area
  std::string functionList = FunctionList(SelectedTextArea());

  // Update list if it has changed
  jumpEnabled = false;
  if ( functionList != lastFunctionList ) {
    // Clear combo box
    while ( qCountComboBoxItems(functionCombo) > 1 )
      qRemoveComboBoxItem(functionCombo, 1);

    // Clear function lines
    functionLines.clear();

    // Add functions
    std::vector<std::string> functions = stringutils::split(functionList, '\n');
    for ( std::vector<std::string>::const_iterator it = functions.begin(); it != functions.end(); it++ ) {
      std::vector<std::string> func = stringutils::split(*it, '@');
      qAddComboBoxItem(functionCombo, func[0].c_str());
      functionLines.push_back(stringutils::intValue(func[1]));
    }

    lastFunctionList = functionList;
  }
  jumpEnabled = true;

  // Save time
  functionUpdateLastMillisecs = Millisecs();
}

std::string MainController::FunctionList(TextArea *textArea) {
  std::vector<std::string> lines = stringutils::split(qEditorText(textArea->GetWidget()), '\n');
  std::string functions = "";
  for ( unsigned int i = 0; i < lines.size(); i++ ) {
    std::string line = stringutils::trim(lines[i]);
    if ( stringutils::find(stringutils::toLowerCase(line), "function ") == 0 ) {
      if ( functions != "" ) functions += "\n";
      std::string funcname = stringutils::split(stringutils::trim(stringutils::rightString(line, line.length() - std::string("function ").length())), '(')[0];
      functions += funcname + "@" + stringutils::fromInt(i + 1);
    }
  }
  return functions;
}

std::string CurrentDir() {
  char buf[FILENAME_MAX];
  getcwd(buf, FILENAME_MAX);
  return std::string(buf);
}

void ChangeDir(const std::string& path) {
  chdir(path.c_str());
}

int RunCommand(const std::string& command, const std::string& filename, const std::string& appPath) {
  // Fix command
  std::string cmdline = stringutils::replace(command, "%SEF", stringutils::stripExt(filename));
  cmdline = stringutils::replace(cmdline, "%F", filename);
  cmdline = stringutils::replace(cmdline, "%P", appPath);

  // Run command
  std::string prevDir = CurrentDir();
  ChangeDir(stringutils::extractPath(filename));
  int ret = system(cmdline.c_str());
  ChangeDir(prevDir);

  return ret;
}
