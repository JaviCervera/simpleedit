#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "controller.h"
#include <string>
#include <vector>

class TextArea;

struct WidgetLayout {
    void* widget;
    int fixedHeight;
    WidgetLayout(void* widget, int fixedHeight) : widget(widget), fixedHeight(fixedHeight) {}
};

class MainController : public Controller {
public:
    MainController(int argc, char* argv[]);
    virtual ~MainController();

    bool IsInitialized();
    void AddWidget(void* widget, int fixedHeight = 0);
    void RemoveWidget(void* widget);
    bool Find(const std::string& text, bool matchCase);
    TextArea* SelectedTextArea();

    // Inherited from Controller
    virtual void WindowClosed(void* win);
    virtual void WindowResized(void* win);
    virtual void MenuIdSelected(int id);
    virtual void WidgetPerformedAction(void *widget, int id);
protected:
    // Menu events
    void DoNew(std::string filename = "");
    void DoOpen(std::string filename = "");
    bool DoSave(bool checkModified = true);
    bool DoSaveAs();
    void DoSaveAll();
    bool DoClose();
    bool DoCloseAll();
    void DoNextFile();
    void DoPrevFile();
    bool DoFindNext(bool wrap = true);
    void DoLockFile();

    void SelectTextArea(TextArea* textArea);
    void SelectBuildTextArea();
    TextArea* BuildTextArea();
    void SetStatus(std::string status);
    void UpdateStatusBar();
    int ClientY();
    int ClientHeight();
    void Layout();
    void UpdateFunctionCombo();
    std::string FunctionList(TextArea* textArea);
private:
    void* win;
    void* menubar;
    void* toolbar;
    void* tabbar;
    void* statusbar;
    void* functionCombo;

    std::string statusText;
    int buildTab;
    int functionUpdateLastMillisecs;
    std::string lastFunctionList;
    std::vector<int> functionLines;
    bool jumpEnabled;   // We set this to false when update the function combo box
    std::vector<WidgetLayout> widgets;
    std::string findText;
    bool matchCase;
    std::string path;
};

#endif // MAINCONTROLLER_H
