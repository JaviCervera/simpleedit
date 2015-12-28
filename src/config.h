#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
public:
    Config(const std::string& filename);
    ~Config();

    bool IsValid() const { return isValid; }
    const std::string& WinTitle() const { return winTitle; }
    const std::string& About() const { return about; }
    const std::string& FileFilter() const { return fileFilter; }
    const std::string& DefaultExt() const { return defaultExt; }
    int TabSize() const { return tabSize; }
    const void* Font() const { return font; }
    int ColorBackground() const { return colorBackground; }
    int ColorForeground() const { return colorForeground; }
    int ColorComments() const { return colorComments; }
    int ColorStrings() const { return colorStrings; }
    int ColorKeywords() const { return colorKeywords; }
    int ColorNumbers() const { return colorNumbers; }
    int ColorPreprocessor() const { return colorPreprocessor; }
    int ColorSymbols() const { return colorSymbols; }
    void RegisterFont();
private:
    bool isValid;
    std::string winTitle;
    std::string about;
    std::string fileFilter;
    std::string defaultExt;
    int tabSize;
    std::string registerFont;
    std::string fontName;
    int fontSize;
    bool fontBold;
    bool fontItalic;
    bool fontUnderline;
    void* font;
    int colorBackground;
    int colorForeground;
    int colorComments;
    int colorStrings;
    int colorKeywords;
    int colorNumbers;
    int colorPreprocessor;
    int colorSymbols;
};

#endif // CONFIG_H
