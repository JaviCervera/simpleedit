#include "qtiny.h"
#include "eventdispatcher.h"
#include "../stringutils.hpp"
#include <map>
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QFrame>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStatusBar>
#include <QSyntaxHighlighter>
#include <QTabWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QToolBar>

#ifdef BUILD_SHARED
#include <qplugin.h>
#if defined(_WIN32)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QWindowsPrinterSupportPlugin)
#elif defined(__APPLE__)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
Q_IMPORT_PLUGIN(QCocoaPrinterSupportPlugin)
#endif
#endif

extern "C" {

void DummyCallback(int event, void* gadget, int id) {}

class Highlighter;

QApplication* app = NULL;
qEventCallback appCallback = DummyCallback;
qEventDispatcher eventDispatcher;

class TWindow : public QMainWindow {
public:
    TWindow(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {}

protected:
    virtual void closeEvent(QCloseEvent *event) {
        appCallback(QEVENT_WINDOWCLOSE, (void*)this, 0);
        event->ignore();

        /*
        // *** Workaround for Mac OS X
#ifdef Q_OS_MAC
        QTimer::singleShot(0,this,SLOT(hide()));
        QTimer::singleShot(100,this,SLOT(show()));
#endif
        */
    }

    virtual void resizeEvent(QResizeEvent *) {
        appCallback(QEVENT_WINDOWSIZE, (void*)this, 0);
    }
};

class Highlighter : public QSyntaxHighlighter {
public:
    Highlighter(QTextDocument* parent = 0) : QSyntaxHighlighter(parent) {
        supportsMultiLineComments = false;
        multiLineCommentFormat.setForeground(QColor(0xFFFF0000));
        commentStartExp = QRegExp("/\\*");
        commentEndExp = QRegExp("\\*/");
    }

    void AddRule(const std::string& rulestr, bool caseSensitive, int color) {
        Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        QTextCharFormat format;
        format.setForeground(QColor(color));

        HighlightingRule rule;
        rule.pattern = QRegExp(rulestr.c_str(), cs);
        rule.format = format;

        rules.push_back(rule);
    }

    void SetMultiLineComments(const std::string& start, const std::string& end, int color) {
        supportsMultiLineComments = true;
        multiLineCommentFormat.setForeground(QColor(color));
        commentStartExp = QRegExp(start.c_str());
        commentEndExp = QRegExp(end.c_str());
    }

protected:
    void highlightBlock(const QString& text) {
        // Apply rules
        for ( std::vector<HighlightingRule>::iterator it = rules.begin(); it != rules.end(); it++ ) {
            QRegExp exp((*it).pattern);
            int index = exp.indexIn(text);
            while ( index >= 0 ) {
                int length = exp.matchedLength();
                setFormat(index, length, (*it).format);
                index = exp.indexIn(text, index + length);
            }
        }

        // Special case for multiline comments
        if ( supportsMultiLineComments ) {
            setCurrentBlockState(0);
            int startIndex = 0;
            if ( previousBlockState() != 1 ) {
                startIndex = commentStartExp.indexIn(text);
            }

            while ( startIndex >= 0 ) {
                int endIndex = commentEndExp.indexIn(text, startIndex);
                int commentLength;
                if ( endIndex == -1 ) {
                    setCurrentBlockState(1);
                    commentLength = text.length() - startIndex;
                } else {
                    commentLength = endIndex - startIndex + commentEndExp.matchedLength();
                }
                setFormat(startIndex, commentLength, multiLineCommentFormat);
                startIndex = commentStartExp.indexIn(text, startIndex + commentLength);
            }
        }
    }
private:
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };
    std::vector<HighlightingRule> rules;

    bool supportsMultiLineComments;
    QRegExp commentStartExp;
    QRegExp commentEndExp;
    QTextCharFormat multiLineCommentFormat;
};

EXPORT void CALL qAppInit(int argc, char** argv) {
    app = new QApplication(argc, argv);
}

EXPORT int CALL qAppRun(qEventCallback callback) {
    if ( callback ) appCallback = callback;
    return app->exec();
}

EXPORT void CALL qAppExit() {
    app->quit();
}

EXPORT void CALL qAppCleanup() {
    delete app;
}

EXPORT void CALL qSetButtonText(void* button, const char* text) {
    ((QPushButton*)button)->setText(text);
}

EXPORT void* CALL qCreateButton(void* parent, const char* text) {
    QPushButton* button = new QPushButton(text, (QWidget*)parent);
    QObject::connect(button, SIGNAL(clicked(bool)), &eventDispatcher, SLOT(ButtonPressed(bool)));
    return button;
}

EXPORT const char* CALL qButtonText(void* button) {
    static std::string str = "";
    str = ((QPushButton*)button)->text().toStdString();
    return str.c_str();
}

EXPORT void* CALL qCreateCheckBox(void* parent, const char* text) {
    QCheckBox* checkbox = new QCheckBox(text, (QWidget*)parent);
    QObject::connect(checkbox, SIGNAL(clicked(bool)), &eventDispatcher, SLOT(ButtonPressed(bool)));
    return checkbox;
}

EXPORT int CALL qCheckBoxState(void* checkbox) {
    switch ( ((QCheckBox*)checkbox)->checkState() ) {
    case Qt::Checked:
        return QCHECKBOX_CHECK;
        break;
    case Qt::PartiallyChecked:
        return QCHECKBOX_PARTIALCHECK;
        break;
    default:
        return QCHECKBOX_UNCHECK;
        break;
    }
}

EXPORT void CALL qSetCheckBoxState(void* checkbox, int state) {
    switch ( state ) {
    case QCHECKBOX_UNCHECK:
        ((QCheckBox*)checkbox)->setCheckState(Qt::Unchecked);
        break;
    case QCHECKBOX_CHECK:
        ((QCheckBox*)checkbox)->setCheckState(Qt::Checked);
        break;
    case QCHECKBOX_PARTIALCHECK:
        ((QCheckBox*)checkbox)->setCheckState(Qt::PartiallyChecked);
        break;
    }
}

EXPORT void CALL qAddEditorRule(void* editor, const char* rule, BOOL caseSensitive, int color) {
    QVariant var = ((QTextEdit*)editor)->property("highlighter");
    Highlighter* highlighter = (Highlighter*)var.value<void*>();
    if ( highlighter )
        highlighter->AddRule(rule, caseSensitive, color);
}

EXPORT void CALL qSetEditorLineWrap(void* editor, int wrap) {
    if ( wrap )
        ((QTextEdit*)editor)->setLineWrapMode(QTextEdit::WidgetWidth);
    else
        ((QTextEdit*)editor)->setLineWrapMode(QTextEdit::NoWrap);
}

EXPORT void CALL qSetEditorMultiLineComments(void* editor, const char* start, const char* end, int color) {
    QVariant var = ((QTextEdit*)editor)->property("highlighter");
    Highlighter* highlighter = (Highlighter*)var.value<void*>();
    if ( highlighter )
        highlighter->SetMultiLineComments(start, end, color);
}

EXPORT void CALL qSetEditorFont(void* editor, void* font) {
    ((QTextEdit*)editor)->setFont(*((QFont*)font));
}

EXPORT void CALL qSetEditorTabSize(void* editor, int tabsize) {
    QFontMetrics metrics(((QTextEdit*)editor)->font());
    ((QTextEdit*)editor)->setTabStopWidth(tabsize * metrics.width(' '));
}

EXPORT void CALL qSetEditorText(void* editor, const char* text) {
    ((QTextEdit*)editor)->setText(text);
}

EXPORT int CALL qEditorLine(void* editor) {
    QTextCursor cursor = ((QTextEdit*)editor)->textCursor();
    return cursor.blockNumber();
}

EXPORT int CALL qEditorColumn(void* editor) {
    QTextCursor cursor = ((QTextEdit*)editor)->textCursor();
    return cursor.columnNumber();
}

EXPORT int CALL qEditorCursorPos(void* editor) {
    return ((QTextEdit*)editor)->textCursor().position();
}

EXPORT void CALL qSetEditorCursorPos(void* editor, int pos) {
    QTextCursor cursor = ((QTextEdit*)editor)->textCursor();
    cursor.setPosition(pos);
    ((QTextEdit*)editor)->setTextCursor(cursor);
}

EXPORT void CALL qSelectEditorText(void* editor, int pos, int length) {
    QTextCursor cursor = ((QTextEdit*)editor)->textCursor();
    cursor.setPosition(pos);
    cursor.setPosition(pos+length, QTextCursor::KeepAnchor);
    ((QTextEdit*)editor)->setTextCursor(cursor);
}

EXPORT int CALL qEditorSelectionPos(void* editor) {
    QTextCursor cursor = ((QTextEdit*)editor)->textCursor();
    return cursor.anchor();
}

EXPORT int CALL qEditorSelectionLen(void* editor) {
    QTextCursor cursor = ((QTextEdit*)editor)->textCursor();
    return abs(cursor.position() - cursor.anchor());
}

EXPORT const char* CALL qEditorText(void* editor) {
    static std::string str = "";
    str = ((QTextEdit*)editor)->toPlainText().toStdString();
    return str.c_str();
}

EXPORT void CALL qEditorCut(void* editor) {
    ((QTextEdit*)editor)->cut();
}

EXPORT void CALL qEditorCopy(void* editor) {
    ((QTextEdit*)editor)->copy();
}

EXPORT void CALL qEditorPaste(void* editor) {
    ((QTextEdit*)editor)->paste();
}

EXPORT void CALL qEditorUndo(void* editor) {
    ((QTextEdit*)editor)->undo();
}

EXPORT void CALL qEditorRedo(void* editor) {
    ((QTextEdit*)editor)->redo();
}

EXPORT int CALL qEditorUndoSteps(void* editor) {
    return ((QTextEdit*)editor)->document()->availableUndoSteps();
}

EXPORT int CALL qEditorRedoSteps(void* editor) {
    return ((QTextEdit*)editor)->document()->availableRedoSteps();
}

EXPORT void CALL qSetLabelText(void* label, const char* text) {
    ((QLabel*)label)->setText(text);
}

EXPORT const char* CALL qLabelText(void* label) {
    static std::string str;
    str = ((QLabel*)label)->text().toStdString();
    return str.c_str();
}

EXPORT void CALL qSetLineEditText(void* lineedit, const char* text) {
    ((QLineEdit*)lineedit)->setText(text);
}

EXPORT const char* CALL qLineEditText(void* lineedit) {
    static std::string str;
    str = ((QLineEdit*)lineedit)->text().toStdString();
    return str.c_str();
}

EXPORT void* CALL qAddMenuTitle(void *menubar, const char* title) {
    return (void*)((QMenuBar*)menubar)->addMenu(title);
}

EXPORT void CALL qAddMenuItem(void *menu, int id, const char* text, const char* shortcut) {
    QAction* action = ((QMenu*)menu)->addAction(text, &eventDispatcher, SLOT(MenuAction()), QKeySequence(shortcut));
    action->setProperty("id", QVariant::fromValue(id));
}

EXPORT void CALL qAddMenuSeparator(void *menu) {
    ((QMenu*)menu)->addSeparator();
}

EXPORT void CALL qSetStatusBarText(void* statusbar, const char* text) {
    ((QLabel*)((QStatusBar*)statusbar)->property("label").value<void*>())->setText(text);
}

EXPORT void CALL qAddTabBarItem(void* tabbar, void* item, const char* text) {
    ((QTabWidget*)tabbar)->addTab((QWidget*)item, text);
}

EXPORT void CALL qRemoveTabBarItem(void* tabbar, int index) {
    ((QTabWidget*)tabbar)->removeTab(index);
}

EXPORT int CALL qCountTabBarItems(void* tabbar) {
    return ((QTabWidget*)tabbar)->count();
}

EXPORT void CALL qSetTabBarIndex(void* tabbar, int index) {
    ((QTabWidget*)tabbar)->setCurrentIndex(index);
}

EXPORT int CALL qTabBarIndex(void* tabbar) {
    return ((QTabWidget*)tabbar)->currentIndex();
}

EXPORT void* CALL qTabBarWidget(void* tabbar, int index) {
    return (void*)((QTabWidget*)tabbar)->widget(index);
}

EXPORT void CALL qSetTabText(void* tabbar, int index, const char* text) {
    ((QTabWidget*)tabbar)->setTabText(index, text);
}

EXPORT const char* CALL qTabText(void* tabbar, int index) {
    static std::string str;
    str = ((QTabWidget*)tabbar)->tabText(index).toStdString();
    return str.c_str();
}

EXPORT void CALL qAddToolBarButton(void* toolbar, int id, void *icon, const char* tooltip) {
    QIcon& iconref = *((QIcon*)icon);
    QAction* action = ((QToolBar*)toolbar)->addAction(iconref, "");
    action->setProperty("id", QVariant::fromValue(id));
    if ( tooltip ) action->setToolTip(tooltip);
    QObject::connect(action, SIGNAL(triggered(bool)), &eventDispatcher, SLOT(MenuAction()));
}

EXPORT void CALL qAddToolBarSeparator(void* toolbar) {
    ((QToolBar*)toolbar)->addSeparator();
}

EXPORT void CALL qAddToolBarWidget(void* toolbar, void* widget) {
    ((QToolBar*)toolbar)->addWidget((QWidget*)widget);
}

EXPORT void CALL qNotify(void* parent, const char* title, const char* text, int warningLevel) {
    switch ( warningLevel ) {
    case QNOTIFY_INFORMATION:
        QMessageBox::information((QWidget*)parent, title, text);
        break;
    case QNOTIFY_WARNING:
        QMessageBox::warning((QWidget*)parent, title, text);
        break;
    case QNOTIFY_ERROR:
        QMessageBox::critical((QWidget*)parent, title, text);
        break;
    }
}

EXPORT int CALL qConfirm(void* parent, const char* title, const char* text, BOOL cancelButton) {
    QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No;
    if ( cancelButton )
        buttons |= QMessageBox::Cancel;
    switch ( QMessageBox::question((QWidget*)parent, title, text, buttons) ) {
    case QMessageBox::No:
        return QCONFIRM_NO;
    case QMessageBox::Yes:
        return QCONFIRM_YES;
    default:
        return QCONFIRM_CANCEL;
    }
}

EXPORT const char* CALL qRequestFile(void* parent, const char* caption, const char* dir, const char* filter, BOOL save) {
    static std::string str = "";
    if ( save ) {
        str = QFileDialog::getSaveFileName((QWidget*)parent, caption, dir, filter).toStdString();
    } else {
        str = QFileDialog::getOpenFileName((QWidget*)parent, caption, dir, filter).toStdString();
    }
    return str.c_str();
}

EXPORT void* CALL qLoadIcon(const char* filename) {
    return (void*)new QIcon(filename);
}

EXPORT void CALL qFreeIcon(void *icon) {
    delete (QIcon*)icon;
}

EXPORT int CALL qRegisterFont(const char* file) {
    return QFontDatabase::addApplicationFont(file);
}

EXPORT void CALL qUnregisterFont(int id) {
    QFontDatabase::removeApplicationFont(id);
}

EXPORT void* CALL qLoadFont(const char* family, int size, BOOL bold, BOOL italic, BOOL underline) {
    QFont* font = new QFont(family);
    font->setPointSize(size);
    font->setBold(bold);
    font->setItalic(italic);
    font->setUnderline(underline);
    return (void*)font;
}

EXPORT void CALL qFreeFont(void* font) {
    delete (QFont*)font;
}

EXPORT void* CALL qCreateFrame(void* parent, int style) {
    return (void*)new QFrame((QWidget*)parent, (Qt::WindowFlags)style);
}

EXPORT void* CALL qRunProgram(const char* program, const char* dir, BOOL wait) {
    // Parse program name and arguments from first parameter
    std::vector<std::string> cmdline = stringutils::split(program, ' ');
    QString command = cmdline[0].c_str();
    QString param = "";
    QStringList params;
    for ( int i = 1; i < cmdline.size(); i++ ) {
        param += cmdline[i].c_str();

        // If we find quotes, match with closing quote
        if ( param.left(1) == "\"" ) {
            if (param.right(1) == "\"" ) {
                params << param.replace("\"", "");
                param = "";
            }
        // Else, add new param
        } else {
            params << param;
            param = "";
        }
    }

    QProcess* process = new QProcess();
    QObject::connect(process, SIGNAL(error(QProcess::ProcessError)), &eventDispatcher, SLOT(ProgramError(QProcess::ProcessError)));
    QObject::connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), &eventDispatcher, SLOT(ProgramFinished(int,QProcess::ExitStatus)));
    QObject::connect(process, SIGNAL(readyRead()), &eventDispatcher, SLOT(ProgramOutputAvailable()));
    process->setWorkingDirectory(dir);
    process->start(command, params);

    return (void*)process;
}

EXPORT void CALL qFreeProgram(void* program) {
    qTerminateProgram(program);
    delete (QProcess*)program;
}

EXPORT void CALL qTerminateProgram(void* program) {
    ((QProcess*)program)->terminate();
    ((QProcess*)program)->kill();
}

EXPORT const char* CALL qProgramStandardOutput(void* program) {
    static std::string str = "";
    str = ((QProcess*)program)->readAllStandardOutput().constData();
    return str.c_str();
}

EXPORT const char* CALL qProgramErrorOutput(void* program) {
    static std::string str = "";
    str = ((QProcess*)program)->readAllStandardError().constData();
    return str.c_str();
}

EXPORT void* CALL qCreateEditor(void *parent, BOOL readonly, BOOL supportHighlighting) {
    QTextEdit* editor = new QTextEdit((QWidget*)parent);
    if ( readonly )
        editor->setReadOnly(true);
    if ( supportHighlighting ) {
        Highlighter* highlighter = new Highlighter(editor->document());
        editor->setProperty("highlighter", QVariant::fromValue((void*)highlighter));
    }
    editor->setLineWrapMode(QTextEdit::NoWrap);
    editor->setPlainText("");
    QObject::connect(editor, SIGNAL(textChanged()), &eventDispatcher, SLOT(EditorTextChanged()));
    QObject::connect(editor, SIGNAL(cursorPositionChanged()), &eventDispatcher, SLOT(EditorCursorMoved()));
    return editor;
}

EXPORT void* CALL qCreateLabel(void* parent, const char* text) {
    return (void*)new QLabel(text, (QWidget*)parent);
    return NULL;
}

EXPORT void* CALL qCreateLineEdit(void* parent, const char* text) {
    QLineEdit* lineedit = new QLineEdit(text, (QWidget*)parent);
    QObject::connect(lineedit, SIGNAL(textChanged(QString)), &eventDispatcher, SLOT(LineEditTextChanged(QString)));
    return lineedit;
}

EXPORT void* CALL qCreateMenuBar(void *win) {
    return (void*)((QMainWindow*)win)->menuBar();
}

EXPORT void* CALL qCreateTabBar(void* parent) {
    QTabWidget* tabbar = new QTabWidget((QWidget*)parent);
    QObject::connect(tabbar, SIGNAL(currentChanged(int)), &eventDispatcher, SLOT(TabBarChangedIndex(int)));
    return (void*)tabbar;
}

EXPORT void* CALL qCreateToolBar(void *win) {
    QToolBar* toolbar = ((QMainWindow*)win)->addToolBar("");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(16, 16));
#ifdef Q_OS_MAC
    toolbar->resize(toolbar->size().width(), toolbar->size().height()-2);
#else
    toolbar->resize(toolbar->size().width(), 16);
#endif
    return (void*)toolbar;
}

EXPORT void* CALL qCreateStatusBar(void* win) {
    QStatusBar* statusbar = ((QMainWindow*)win)->statusBar();
    statusbar->setStyleSheet("QStatusBar::item { border: none; }");
    QLabel* label = new QLabel();
    statusbar->addWidget(label);
    statusbar->setProperty("label", QVariant::fromValue((void*)label));
    return (void*)statusbar;
}

EXPORT void CALL qFreeWidget(void* widget) {
    // If it's a textedit and has an associated highlighter, delete it
    QVariant var = ((QTextEdit*)widget)->property("highlighter");
    Highlighter* highlighter = (Highlighter*)var.value<void*>();
    if ( highlighter ) delete highlighter;

    delete (QWidget*)widget;
}

EXPORT void CALL qLayoutWidget(void* widget, int x, int y, int w, int h) {
    ((QWidget*)widget)->move(x, y);
    ((QWidget*)widget)->resize(w, h);
}

EXPORT int CALL qWidgetX(void* widget) {
    return ((QWidget*)widget)->pos().x();
}

EXPORT int CALL qWidgetY(void *widget) {
    return ((QWidget*)widget)->pos().y();
}

EXPORT int CALL qWidgetWidth(void *widget) {
    return ((QWidget*)widget)->size().width();
}

EXPORT int CALL qWidgetHeight(void *widget) {
    return ((QWidget*)widget)->size().height();
}

EXPORT void CALL qEnableWidget(void* widget, BOOL enable) {
    ((QWidget*)widget)->setEnabled(enable);
}

EXPORT void CALL qShowWidget(void *widget, BOOL show) {
    ((QWidget*)widget)->setVisible((bool)show);
}

EXPORT void CALL qFocusWidget(void* widget) {
    ((QWidget*)widget)->setFocus(Qt::OtherFocusReason);
}

EXPORT void* CALL qWidgetOSHandle(void* widget) {
    return (void*)((QWidget*)widget)->winId();
}

EXPORT void CALL qSetWidgetStyleSheet(void* widget, const char* sheet) {
    ((QWidget*)widget)->setStyleSheet(sheet);
}

EXPORT void CALL qSetWidgetProperty(void* widget, const char* property, void* value) {
    ((QWidget*)widget)->setProperty(property, QVariant::fromValue(value));
}

EXPORT void* CALL qWidgetProperty(void* widget, const char* property) {
    return ((QWidget*)widget)->property(property).value<void*>();
}

EXPORT void* CALL qCreateWindow(void* parent, const char* title, int x, int y, int w, int h, int flags) {
    Qt::WindowFlags qtFlags = 0;
    if ( flags & QWINDOW_TOOL ) qtFlags |= Qt::Tool;
    TWindow* win = new TWindow((QWidget*)parent, qtFlags);
    //win->setUnifiedTitleAndToolBarOnMac(true);
    if ( flags & QWINDOW_MODAL ) win->setWindowModality(Qt::ApplicationModal);
    if ( flags & QWINDOW_SHEET ) win->setWindowModality(Qt::WindowModal);
    if ( flags & QWINDOW_CENTERED ) {
        if ( parent ) {
            int wdiff = qWidgetWidth(parent) - w;
            int hdiff = qWidgetHeight(parent) - h;
            x = qWidgetX(parent) + wdiff/2;
            y = qWidgetY(parent) + hdiff/2;
        } else {
            QDesktopWidget* desktop = QApplication::desktop();
            x = (desktop->width() - w) / 2;
            y = (desktop->height() - h) / 2;
        }
    }
    win->move(x, y);
    win->resize(w, h);
    win->setWindowTitle(title);
    return win;
}

EXPORT void CALL qSetWindowTitle(void* win, const char* title) {
    ((QMainWindow*)win)->setWindowTitle(title);
}

EXPORT void* CALL qCreateComboBox(void* parent) {
    QComboBox* combobox = new QComboBox((QWidget*)parent);
    combobox->setDuplicatesEnabled(true);
    QObject::connect(combobox, SIGNAL(currentIndexChanged(int)), &eventDispatcher, SLOT(TabBarChangedIndex(int)));
    return combobox;
}

EXPORT void CALL qAddComboBoxItem(void* combobox, const char* text) {
    ((QComboBox*)combobox)->addItem(text);
}

EXPORT int CALL qCountComboBoxItems(void* combobox) {
    return ((QComboBox*)combobox)->count();
}

EXPORT void CALL qRemoveComboBoxItem(void* combobox, int index) {
    ((QComboBox*)combobox)->removeItem(index);
}

EXPORT void CALL qSetComboBoxIndex(void* combobox, int index) {
    ((QComboBox*)combobox)->setCurrentIndex(index);
}

EXPORT void CALL qSetComboBoxText(void* combobox, int index, const char* text) {
    ((QComboBox*)combobox)->setItemText(index, text);
}

EXPORT int CALL qComboBoxIndex(void* combobox) {
    return ((QComboBox*)combobox)->currentIndex();
}

EXPORT const char* CALL qComboBoxText(void* combobox, int index) {
    static std::string str = "";
    str = ((QComboBox*)combobox)->itemText(index).toStdString();
    return str.c_str();
}

EXPORT const char* CALL qLoadUTF8String(const char* filename) {
  static QByteArray localRep;
  QString str;
  QFile file(filename);
  if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
    QTextStream in(&file);
    in.setCodec("UTF-8");
    str = in.readAll();
  }
  localRep = str.toUtf8();
  return localRep.constData();
}

} // extern "C"
