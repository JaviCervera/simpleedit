#ifndef PROGRAM_H
#define PROGRAM_H

#include "controller.h"
#include <string>
#include <vector>

class CompileController : public Controller {
public:
    CompileController(void* parent, const std::string& appPath);
    virtual ~CompileController();

    void Run(const std::vector<std::string>& commands, const std::string &filename, const std::string& appPath);

    // Inherited
    virtual void WindowClosed(void* win);
    virtual void WidgetPerformedAction(void* widget, int id);
    virtual void ProgramError(void* program, bool crash);
    virtual void ProgramFinished(void* program);
    virtual void ProgramOutputAvailable(void* program);
protected:
    bool RunNextCommand();
private:
    Controller* oldController;
    void* win;
    void* editor;
    void* button;
    void* program;
    int currentCommand;
    std::vector<std::string> commands;
    std::string filename;
    std::string appPath;
};

#endif // PROGRAM_H
