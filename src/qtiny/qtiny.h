#ifndef QTINY_H
#define QTINY_H

// Export mode and call convention
#if defined(_WIN32) && defined(BUILD_SHARED)
#define EXPORT	__declspec(dllexport)
#define CALL	_stdcall
#elif defined(_WIN32) && defined(USE_SHARED)
#define EXPORT	__declspec(dllimport)
#define CALL	_stdcall
#else
#define EXPORT
#define CALL
#endif

// BOOL type and values
#define FALSE 0
#define TRUE 1
typedef int BOOL;

// Checkbox states
#define QCHECKBOX_UNCHECK       0
#define QCHECKBOX_CHECK         1
#define QCHECKBOX_PARTIALCHECK  2

// Confirm results
#define QCONFIRM_CANCEL      0
#define QCONFIRM_NO          1
#define QCONFIRM_YES         2

// Events
#define QEVENT_WINDOWCLOSE      1
#define QEVENT_WINDOWSIZE       2
#define QEVENT_MENUACTION       3
#define QEVENT_WIDGETACTION     4
#define QEVENT_PROGRAMFINISH    5
#define QEVENT_PROGRAMOUTPUT    6
#define QEVENT_PROGRAMERROR     7

// Frame styles
#define QFRAME_NOBORDER 0
#define QFRAME_BOX      0x0001
#define QFRAME_PANEL    0x0002
#define QFRAME_STYLED   0x0006
#define QFRAME_PLAIN    0x0010
#define QFRAME_RAISED   0x0020
#define QFRAME_SUNKEN   0x0030

// Notify levels
#define QNOTIFY_INFORMATION 0
#define QNOTIFY_WARNING     1
#define QNOTIFY_ERROR       2

// Window flags
#define QWINDOW_STANDARD    0
#define QWINDOW_TOOL        1
#define QWINDOW_MODAL       2
#define QWINDOW_SHEET       4
#define QWINDOW_CENTERED    8

#ifdef __cplusplus
extern "C" {
#endif

// Event callback
typedef void (*qEventCallback)(int,void*,int);

// App
EXPORT void CALL qAppInit(int argc, char** argv);
EXPORT int CALL qAppRun(qEventCallback callback);
EXPORT void CALL qAppExit();
EXPORT void CALL qAppCleanup();

// Button
EXPORT void* CALL qCreateButton(void* parent, const char* text);
EXPORT const char* CALL qButtonText(void* button);
EXPORT void CALL qSetButtonText(void* button, const char* text);

// Check box
EXPORT void* CALL qCreateCheckBox(void* parent, const char* text);
EXPORT int CALL qCheckBoxState(void* checkbox);
EXPORT void CALL qSetCheckBoxState(void* checkbox, int state);

// Combo box
EXPORT void* CALL qCreateComboBox(void* parent);
EXPORT void CALL qAddComboBoxItem(void* combobox, const char* text);
EXPORT int CALL qCountComboBoxItems(void* combobox);
EXPORT void CALL qRemoveComboBoxItem(void* combobox, int index);
EXPORT void CALL qSetComboBoxIndex(void* combobox, int index);
EXPORT void CALL qSetComboBoxText(void* combobox, int index, const char* text);
EXPORT int CALL qComboBoxIndex(void* combobox);
EXPORT const char* CALL qComboBoxText(void* combobox, int index);

// Editor
EXPORT void* CALL qCreateEditor(void* parent, BOOL readonly, BOOL supportHighlighting);
EXPORT void CALL qAddEditorRule(void* editor, const char* rule, BOOL caseSensitive, int color);
EXPORT int CALL qEditorColumn(void* editor);
EXPORT void CALL qEditorCopy(void* editor);
EXPORT int CALL qEditorCursorPos(void* editor);
EXPORT void CALL qEditorCut(void* editor);
EXPORT int CALL qEditorLine(void* editor);
EXPORT void CALL qEditorPaste(void* editor);
EXPORT int CALL qEditorSelectionLen(void* editor);
EXPORT int CALL qEditorSelectionPos(void* editor);
EXPORT void CALL qEditorRedo(void* editor);
EXPORT int CALL qEditorRedoSteps(void* editor);
EXPORT const char* CALL qEditorText(void* editor);
EXPORT void CALL qEditorUndo(void* editor);
EXPORT int CALL qEditorUndoSteps(void* editor);
EXPORT void CALL qSelectEditorText(void* editor, int pos, int length);
EXPORT void CALL qSetEditorCursorPos(void* editor, int pos);
EXPORT void CALL qSetEditorFont(void* editor, void* font);
EXPORT void CALL qSetEditorLineWrap(void* editor, int wrap);
EXPORT void CALL qSetEditorMultiLineComments(void* editor, const char* start, const char* end, int color);
EXPORT void CALL qSetEditorTabSize(void* editor, int tabsize);
EXPORT void CALL qSetEditorText(void* editor, const char* text);

// Font
EXPORT int CALL qRegisterFont(const char* file);
EXPORT void CALL qUnregisterFont(int id);
EXPORT void* CALL qLoadFont(const char* family, int size, BOOL bold, BOOL italic, BOOL underline);
EXPORT void CALL qFreeFont(void* font);

// Frame
EXPORT void* CALL qCreateFrame(void* parent, int style);

// Icon
EXPORT void* CALL qLoadIcon(const char* filename);
EXPORT void CALL qFreeIcon(void* icon);

// Label
EXPORT void* CALL qCreateLabel(void* parent, const char* text);
EXPORT const char* CALL qLabelText(void* label);
EXPORT void CALL qSetLabelText(void* label, const char* text);

// Line edit
EXPORT void* CALL qCreateLineEdit(void* parent, const char* text);
EXPORT const char* CALL qLineEditText(void* lineedit);
EXPORT void CALL qSetLineEditText(void* lineedit, const char* text);

// Menu
EXPORT void* CALL qCreateMenuBar(void* win);
EXPORT void CALL qAddMenuItem(void* menu, int id, const char* text, const char *shortcut);
EXPORT void CALL qAddMenuSeparator(void* menu);
EXPORT void* CALL qAddMenuTitle(void* menu, const char* title);

// Requester
EXPORT int CALL qConfirm(void* parent, const char* title, const char* text, BOOL cancelButton);
EXPORT void CALL qNotify(void* parent, const char* title, const char* text, int warningLevel);
EXPORT const char* CALL qRequestFile(void* parent, const char* caption, const char* dir, const char* filter, BOOL save);

// Status bar
EXPORT void* CALL qCreateStatusBar(void* win);
EXPORT void CALL qSetStatusBarText(void* statusbar, const char* text);

// String
EXPORT const char* CALL qLoadUTF8String(const char* filename);

// Tab bar
EXPORT void* CALL qCreateTabBar(void* parent);
EXPORT void CALL qAddTabBarItem(void* tabbar, void* item, const char* text);
EXPORT int CALL qCountTabBarItems(void* tabbar);
EXPORT void CALL qRemoveTabBarItem(void* tabbar, int index);
EXPORT void CALL qSetTabBarIndex(void* tabbar, int index);
EXPORT void CALL qSetTabText(void* tabbar, int index, const char* text);
EXPORT int CALL qTabBarIndex(void* tabbar);
EXPORT const char* CALL qTabText(void* tabbar, int index);
EXPORT void* CALL qTabBarWidget(void* tabbar, int index);

// Tool bar
EXPORT void* CALL qCreateToolBar(void* win);
EXPORT void CALL qAddToolBarButton(void* toolbar, int id, void* icon, const char* tooltip);
EXPORT void CALL qAddToolBarSeparator(void* toolbar);
EXPORT void CALL qAddToolBarWidget(void* toolbar, void* widget);

// Process
EXPORT void* CALL qRunProgram(const char* program, const char* dir, BOOL wait);
EXPORT void CALL qFreeProgram(void* program);
EXPORT void CALL qTerminateProgram(void* program);
EXPORT const char* CALL qProgramStandardOutput(void* program);
EXPORT const char* CALL qProgramErrorOutput(void* program);

// Widget
EXPORT void CALL qFreeWidget(void* widget);
EXPORT void CALL qLayoutWidget(void* widget, int x, int y, int w, int h);
EXPORT int CALL qWidgetX(void* widget);
EXPORT int CALL qWidgetY(void* widget);
EXPORT int CALL qWidgetWidth(void* widget);
EXPORT int CALL qWidgetHeight(void* widget);
EXPORT void CALL qEnableWidget(void* widget, BOOL enable);
EXPORT void CALL qShowWidget(void* widget, BOOL show);
EXPORT void CALL qFocusWidget(void* widget);
EXPORT void* CALL qWidgetOSHandle(void* widget);
EXPORT void CALL qSetWidgetStyleSheet(void* widget, const char* sheet);
EXPORT void CALL qSetWidgetProperty(void* widget, const char* property, void* value);
EXPORT void* CALL qWidgetProperty(void* widget, const char* property);

// Window
EXPORT void* CALL qCreateWindow(void* parent, const char* title, int x, int y, int w, int h, int flags);
EXPORT void CALL qSetWindowTitle(void* win, const char* title);

#ifdef __cplusplus
}
#endif

#endif // QTINY_H
