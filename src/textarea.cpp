#include "textarea.h"
#include "application.h"
#include "config.h"
#include "language.h"
#include "log.h"
#include "qtiny/qtiny.h"
#include "stringutils.hpp"


TextArea::TextArea(const std::string &file) {
    this->file = file;
    this->modified = false;
    widget = qCreateEditor(NULL, FALSE, TRUE);
    qSetWidgetProperty(widget, "textarea", (void*)this);
    Setup();
    if ( file != "" ) {
        std::string fileContents = qLoadUTF8String(file.c_str()); //stringutils::read(file);
        qSetEditorText(widget, fileContents.c_str());
    }
    lastSavedText = qEditorText(widget);
}

std::string TextArea::GetFile() {
    return file;
}

std::string TextArea::GetText() {
    return qEditorText(widget);
}

bool TextArea::IsModified() {
    return modified;
}

void TextArea::SetModified(bool modified) {
    if ( modified == false )
        lastSavedText = qEditorText(widget);
    this->modified = modified;
}

void TextArea::TextChanged() {
    if ( qEditorText(widget) == lastSavedText )
        SetModified(false);
    else
        SetModified(true);
}

void TextArea::Save() {
    stringutils::write(GetFile(), GetText());
    SetModified(false);
}

void TextArea::Setup() {
    // Set font, tabs, colors...
    qSetEditorFont(widget, (void*)Application::Instance()->GetConfig()->Font());
    qSetEditorTabSize(widget, Application::Instance()->GetConfig()->TabSize());
    int fgcolor = Application::Instance()->GetConfig()->ColorForeground();
    int bgcolor = Application::Instance()->GetConfig()->ColorBackground();
    qSetWidgetStyleSheet(widget, ("color: #" + stringutils::hexFromInt(fgcolor) + "; background-color: #" + stringutils::hexFromInt(bgcolor)).c_str());

    // Set multiline comments
    if ( Application::Instance()->GetLanguage()->MultiLineBegin() != "" )
        qSetEditorMultiLineComments(widget, Application::Instance()->GetLanguage()->MultiLineBegin().c_str(), Application::Instance()->GetLanguage()->MultiLineEnd().c_str(), Application::Instance()->GetConfig()->ColorComments());

    // Add keywords
    std::vector<std::string> keywords = stringutils::split(Application::Instance()->GetLanguage()->Keywords(), ' ');
    for ( std::vector<std::string>::iterator it = keywords.begin(); it != keywords.end(); it++ ) {
        std::string rule = "\\b" + *it + "\\b";
        qAddEditorRule(widget, rule.c_str(), Application::Instance()->GetLanguage()->IsCaseSensitive(), Application::Instance()->GetConfig()->ColorKeywords());
    }

    // Add other rules
    const std::vector<Language::SyntaxRule>& rules = Application::Instance()->GetLanguage()->Rules();
    for ( std::vector<Language::SyntaxRule>::const_iterator it = rules.begin(); it != rules.end(); it++ ) {
        int color = 0;
        if ( (*it).id == "comments" ) color = Application::Instance()->GetConfig()->ColorComments();
        else if ( (*it).id == "strings" ) color = Application::Instance()->GetConfig()->ColorStrings();
        else if ( (*it).id == "keywords" ) color = Application::Instance()->GetConfig()->ColorKeywords();
        else if ( (*it).id == "numbers" ) color = Application::Instance()->GetConfig()->ColorNumbers();
        else if ( (*it).id == "preprocessor" ) color = Application::Instance()->GetConfig()->ColorPreprocessor();
        else if ( (*it).id == "symbols" ) color = Application::Instance()->GetConfig()->ColorSymbols();
        qAddEditorRule(widget, (*it).regex.c_str(), true, color);
    }
}

int TextArea::FindNext(const std::string &text, bool matchCase, bool fromBeginning) {
    // Get string from current location
    int ofs = 0;
    std::string str = "";
    if ( fromBeginning ) {
        str = qEditorText(widget);
    } else {
        ofs = qEditorCursorPos(widget);
        str = stringutils::rightString(qEditorText(widget), std::string(qEditorText(widget)).length() - ofs);
    }
    int pos = -1;

    // Search
    if ( matchCase )
        pos = stringutils::find(str, text);
    else
        pos = stringutils::find(stringutils::toLowerCase(str), stringutils::toLowerCase(text));
    if ( pos != -1 )
        pos += ofs;

    return pos;
}

void TextArea::Replace(const std::string &text) {
    std::string txt = qEditorText(widget);
    std::string left = stringutils::leftString(txt, qEditorSelectionPos(widget));
    std::string right = stringutils::rightString(txt, txt.length() - left.length() - text.length());
    qSetEditorText(widget, (left + text + right).c_str());
}
