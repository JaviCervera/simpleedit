#include "eventdispatcher.h"
#include "qtiny.h"
#include <map>
#include <QAction>
#include <QTextCursor>
#include <QTextEdit>

extern "C" qEventCallback appCallback;

qEventDispatcher::~qEventDispatcher() {}

void qEventDispatcher::MenuAction() {
    appCallback(QEVENT_MENUACTION, NULL, sender()->property("id").value<int>());
}

void qEventDispatcher::EditorTextChanged() {
    appCallback(QEVENT_WIDGETACTION, (void*)sender(), 0);
}

void qEventDispatcher::EditorCursorMoved() {
    appCallback(QEVENT_WIDGETACTION, (void*)sender(), 1);
}

void qEventDispatcher::LineEditTextChanged(const QString &text) {
    appCallback(QEVENT_WIDGETACTION, (void*)sender(), 0);
}

void qEventDispatcher::TabBarChangedIndex(int index) {
    appCallback(QEVENT_WIDGETACTION, (void*)sender(), index);
}

void qEventDispatcher::ButtonPressed(bool checked) {
    appCallback(QEVENT_WIDGETACTION, (void*)sender(), (int)checked);
}

void qEventDispatcher::ProgramError(QProcess::ProcessError error) {
    appCallback(QEVENT_PROGRAMERROR, (void*)sender(), error == QProcess::Crashed);
}

void qEventDispatcher::ProgramFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    appCallback(QEVENT_PROGRAMFINISH, (void*)sender(), exitCode);
}

void qEventDispatcher::ProgramOutputAvailable() {
    appCallback(QEVENT_PROGRAMOUTPUT, (void*)sender(), 0);
}
