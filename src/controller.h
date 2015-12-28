#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
public:
    virtual ~Controller() {}
    virtual void WindowClosed(void* win) {}
    virtual void WindowResized(void* win) {}
    virtual void MenuIdSelected(int id) {}
    virtual void WidgetPerformedAction(void* widget, int id) {}
    virtual void ProgramError(void* program, bool crash) {}
    virtual void ProgramFinished(void* program) {}
    virtual void ProgramOutputAvailable(void* program) {}
};

#endif // CONTROLLER_H
