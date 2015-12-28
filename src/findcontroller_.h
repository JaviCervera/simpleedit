#ifndef FINDCONTROLLER_H
#define FINDCONTROLLER_H

#include"controller.h"
#include <string>

class FindController : public Controller {
public:
    FindController(void* parent);
    virtual ~FindController();

    // Inherited
    virtual void WindowClosed(void* win);
    virtual void WidgetPerformedAction(void* widget);
private:
    Controller* prevController;
    void* win;
    void* findButton;
    void* cancelButton;
    void* findLabel;
    void* findLineEdit;
    void* matchCaseCheckBox;
    static std::string findString;
};

#endif // FINDCONTROLLER_H
