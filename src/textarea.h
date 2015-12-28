#ifndef TEXTAREA_H
#define TEXTAREA_H

#include <string>

class TextArea {
public:
    TextArea(const std::string& file);

    void* GetWidget() { return widget; }
    void SetFile(const std::string& file) { this->file = file; }
    std::string GetFile();
    std::string GetText();
    bool IsModified();
    void SetModified(bool modified);
    void TextChanged();
    void Save();
    int FindNext(const std::string& text, bool matchCase, bool fromBeginning = false);
    void Replace(const std::string& text);
protected:
    void Setup();
private:
    void* widget;
    std::string file;
    bool modified;
    std::string lastSavedText;
};

#endif // TEXTAREA_H
