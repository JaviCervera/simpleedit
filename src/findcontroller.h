#ifndef FINDCONTROLLER_H
#define FINDCONTROLLER_H

#include "controller.h"
#include <string>

class FindController : public Controller {
public:
    FindController(void* parent, const std::string& appPath);
    virtual ~FindController();
    void Layout(void* win);

    // Inherited
    virtual void WindowClosed(void* win);
    virtual void WindowResized(void* win);
    virtual void MenuIdSelected(int id);
    virtual void WidgetPerformedAction(void* widget, int id);
    virtual void ProgramError(void* program, bool crash);
    virtual void ProgramFinished(void* program);
    virtual void ProgramOutputAvailable(void* program);
private:
    Controller* prevController;
    void* frame;
    void* findButton;
    void* replaceButton;
    void* replaceAllButton;
    void* cancelButton;
    void* findLabel;
    void* findLineEdit;
    void* replaceLabel;
    void* replaceLineEdit;
    void* matchCaseCheckBox;
    static std::string findString;
};

#endif // FINDCONTROLLER_H
