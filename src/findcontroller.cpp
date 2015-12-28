#include "findcontroller.h"
#include "application.h"
#include "guiloader.h"
#include "maincontroller.h"
#include "qtiny/qtiny.h"
#include "textarea.h"

std::string FindController::findString = "";

FindController::FindController(void* parent, const std::string& appPath) {
    // Save previous controller
    prevController = Application::Instance()->GetController();

    // Load gui
    std::map<std::string, void*> widgets = GuiLoader::Load(appPath + "cfg/find.xml", parent);
    frame = widgets["frame"];
    findButton = widgets["findButton"];
    replaceButton = widgets["replaceButton"];
    replaceAllButton = widgets["replaceAllButton"];
    cancelButton = widgets["cancelButton"];
    findLabel = widgets["findLabel"];
    findLineEdit = widgets["findLineEdit"];
    replaceLabel = widgets["replaceLabel"];
    replaceLineEdit = widgets["replaceLineEdit"];
    matchCaseCheckBox = widgets["matchCaseCheckBox"];

    // Set initial find string
    if ( findString != "" ) {
        qSetLineEditText(findLineEdit, findString.c_str());
    } else {
        qEnableWidget(findButton, false);
        qEnableWidget(replaceButton, false);
        qEnableWidget(replaceAllButton, false);
    }

    // Attach to main controller
    ((MainController*)prevController)->AddWidget(frame, qWidgetHeight(frame));
    Layout(parent);
    qShowWidget(frame, true);
    qFocusWidget(findLineEdit);
}

FindController::~FindController() {
    ((MainController*)prevController)->RemoveWidget(frame);
    qFreeWidget(findButton);
    qFreeWidget(replaceButton);
    qFreeWidget(replaceAllButton);
    qFreeWidget(cancelButton);
    qFreeWidget(findLabel);
    qFreeWidget(findLineEdit);
    qFreeWidget(replaceLabel);
    qFreeWidget(replaceLineEdit);
    qFreeWidget(matchCaseCheckBox);
    qFreeWidget(frame);
}

void FindController::Layout(void* win) {
    qLayoutWidget(frame, qWidgetX(frame), qWidgetY(frame), qWidgetWidth(win), qWidgetHeight(frame));
    qLayoutWidget(cancelButton, qWidgetWidth(win) - 104, qWidgetY(cancelButton), qWidgetWidth(cancelButton), qWidgetHeight(cancelButton));
    qLayoutWidget(findButton, qWidgetWidth(win) - 308, qWidgetY(findButton), qWidgetWidth(findButton), qWidgetHeight(findButton));
    qLayoutWidget(replaceButton, qWidgetWidth(win) - 206, qWidgetY(replaceButton), qWidgetWidth(replaceButton), qWidgetHeight(replaceButton));
    qLayoutWidget(replaceAllButton, qWidgetWidth(win) - 104, qWidgetY(replaceAllButton), qWidgetWidth(replaceAllButton), qWidgetHeight(replaceAllButton));
    qLayoutWidget(matchCaseCheckBox, qWidgetWidth(win) - 308, qWidgetY(matchCaseCheckBox), qWidgetWidth(matchCaseCheckBox), qWidgetHeight(matchCaseCheckBox));
    qLayoutWidget(findLineEdit, qWidgetX(findLineEdit), qWidgetY(findLineEdit), qWidgetWidth(win) - 408, qWidgetHeight(findLineEdit));
    qLayoutWidget(replaceLineEdit, qWidgetX(replaceLineEdit), qWidgetY(replaceLineEdit), qWidgetWidth(win) - 408, qWidgetHeight(replaceLineEdit));
}

void FindController::WindowClosed(void *win) {
    if ( win == frame )
        Application::Instance()->SetController(prevController, true);
    else
        prevController->WindowClosed(win);
}

void FindController::WindowResized(void* win) {
    prevController->WindowResized(win);
    Layout(win);
}

void FindController::MenuIdSelected(int id) {
    prevController->MenuIdSelected(id);
    if ( id == 206 ) WindowClosed(frame);
}

void FindController::WidgetPerformedAction(void *widget, int id) {
    if ( widget == cancelButton ) {
        WindowClosed(frame);
    } else if ( widget == findButton ) {
        ((MainController*)prevController)->Find(qLineEditText(findLineEdit), qCheckBoxState(matchCaseCheckBox) == QCHECKBOX_CHECK);
    } else if ( widget == replaceButton ) {
        ((MainController*)prevController)->SelectedTextArea()->Replace(qLineEditText(replaceLineEdit));
        ((MainController*)prevController)->Find(qLineEditText(findLineEdit), qCheckBoxState(matchCaseCheckBox) == QCHECKBOX_CHECK);
    } else if ( widget == replaceAllButton ) {
        while ( ((MainController*)prevController)->Find(qLineEditText(findLineEdit), qCheckBoxState(matchCaseCheckBox) == QCHECKBOX_CHECK) ) {
            ((MainController*)prevController)->SelectedTextArea()->Replace(qLineEditText(replaceLineEdit));
        }
    } else if ( widget == findLineEdit ) {
        findString = qLineEditText(findLineEdit);
        if ( std::string(qLineEditText(findLineEdit)) != "" ) {
            qEnableWidget(findButton, true);
            qEnableWidget(replaceButton, true);
            qEnableWidget(replaceAllButton, true);
        } else {
            qEnableWidget(findButton, false);
            qEnableWidget(replaceButton, false);
            qEnableWidget(replaceAllButton, false);
        }
    } else {
        prevController->WidgetPerformedAction(widget, id);
    }
}

void FindController::ProgramError(void* program, bool crash) {
    prevController->ProgramError(program, crash);
}

void FindController::ProgramFinished(void* program) {
    prevController->ProgramFinished(program);
}

void FindController::ProgramOutputAvailable(void* program) {
    prevController->ProgramOutputAvailable(program);
}
