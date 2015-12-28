#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <QObject>
#include <QProcess>

class qEventDispatcher : public QObject {
    Q_OBJECT
public:
    virtual ~qEventDispatcher();
public slots:
    void MenuAction();
    void EditorTextChanged();
    void EditorCursorMoved();
    void LineEditTextChanged(const QString& text);
    void TabBarChangedIndex(int index);
    void ButtonPressed(bool checked);
    void ProgramError(QProcess::ProcessError error);
    void ProgramFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void ProgramOutputAvailable();
};

#endif // EVENTDISPATCHER_H
