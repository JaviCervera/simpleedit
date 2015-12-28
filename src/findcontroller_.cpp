#include "findcontroller.h"
#include "application.h"
#include "guiloader.h"
#include "maincontroller.h"
#include "qtiny/qtiny.h"

std::string FindController::findString = "";

FindController::FindController(void* parent) {
    // Save previous controller
    prevController = Application::Instance()->GetController();

    // Load gui
    std::map<std::string, void*> widgets = GuiLoader::Load("cfg/find.xml", parent);
    win = widgets["win"];
    findButton = widgets["findButton"];
    cancelButton = widgets["cancelButton"];
    findLabel = widgets["findLabel"];
    findLineEdit = widgets["findLineEdit"];
    matchCaseCheckBox = widgets["matchCaseCheckBox"];

    // Set initial find string
    if ( findString != "" )
        qSetLineEditText(findLineEdit, findString.c_str());
    else
        qEnableWidget(findButton, false);

    qShowWidget(win, true);
    qFocusWidget(findLineEdit);
}

FindController::~FindController() {
    qFreeWidget(findButton);
    qFreeWidget(cancelButton);
    qFreeWidget(findLabel);
    qFreeWidget(findLineEdit);
    qFreeWidget(matchCaseCheckBox);
    qFreeWidget(win);
}

void FindController::WindowClosed(void *win) {
    if ( win == this->win )
        Application::Instance()->SetController(prevController, true);
}

void FindController::WidgetPerformedAction(void *widget) {
    if ( widget == cancelButton ) {
        WindowClosed(win);
    } else if ( widget == findButton ) {
        ((MainController*)prevController)->Find(qLineEditText(findLineEdit), qCheckBoxState(matchCaseCheckBox) == QCHECKBOX_CHECK);
        WindowClosed(win);
    } else if ( widget == findLineEdit ) {
        findString = qLineEditText(findLineEdit);
        if ( std::string(qLineEditText(findLineEdit)) != "" )
            qEnableWidget(findButton, true);
        else
            qEnableWidget(findButton, false);
    }
}
