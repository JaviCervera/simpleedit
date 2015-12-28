#include "config.h"
#include "log.h"
#include "qtiny/qtiny.h"
#include "rapidxml.hpp"
#include "stringutils.hpp"

using namespace rapidxml;
using namespace stringutils;

Config::Config(const std::string& filename) {
#if defined(_WIN32)
    std::string platform = "win";
#elif defined(__APPLE__)
    std::string platform = "mac";
#elif defined(__linux__)
    std::string platform = "linux";
#endif

    isValid = false;
    font = NULL;

    std::string xmlstr = read(filename);
    if ( xmlstr == "" ) return;

    xml_document<> doc;
    try {
        doc.parse<0>((char*)xmlstr.c_str());
    } catch ( parse_error e ) {
        return;
    }

    xml_node<>* configNode = doc.first_node("config");
    if ( !configNode ) return;

    Log::Write("Parsing global data...");
    winTitle = configNode->first_node("wintitle")->value();
    about = configNode->first_node("about")->value();
    fileFilter = configNode->first_node("filefilter")->value();
    defaultExt = configNode->first_node("defaultext")->value();
    tabSize = intValue(configNode->first_node("tabsize")->value());

    // Check if we have to register a font
    if ( configNode->first_node("font")->first_node("register") ) {
        Log::Write("Registering font...");
        registerFont = configNode->first_node("font")->first_node("register")->value();
        Log::Write("Font '" + registerFont + "' registered");
    }

    Log::Write("Parsing font data...");
    fontName = (configNode->first_node("font")->first_node(("name:" + platform).c_str()))
            ? configNode->first_node("font")->first_node(("name:" + platform).c_str())->value()
            : configNode->first_node("font")->first_node("name")->value();
    fontSize = intValue((configNode->first_node("font")->first_node(("size:" + platform).c_str()))
                            ? configNode->first_node("font")->first_node(("size:" + platform).c_str())->value()
                            : configNode->first_node("font")->first_node("size")->value());
    fontBold = std::string(configNode->first_node("font")->first_node("flags")->first_attribute("bold")->value()) == "true";
    fontItalic = std::string(configNode->first_node("font")->first_node("flags")->first_attribute("italic")->value()) == "true";
    fontUnderline = std::string(configNode->first_node("font")->first_node("flags")->first_attribute("underline")->value()) == "true";
    //font = qLoadFont(fontName.c_str(), fontSize, fontBold, fontItalic, fontUnderline);

    Log::Write("Parsing color data...");
    colorBackground = intValue(configNode->first_node("colors")->first_node("background")->first_attribute("rgb")->value());
    colorForeground = intValue(configNode->first_node("colors")->first_node("foreground")->first_attribute("rgb")->value());
    colorComments = intValue(configNode->first_node("colors")->first_node("comments")->first_attribute("rgb")->value());
    colorStrings = intValue(configNode->first_node("colors")->first_node("strings")->first_attribute("rgb")->value());
    colorKeywords = intValue(configNode->first_node("colors")->first_node("keywords")->first_attribute("rgb")->value());
    colorNumbers = intValue(configNode->first_node("colors")->first_node("numbers")->first_attribute("rgb")->value());
    colorPreprocessor = intValue(configNode->first_node("colors")->first_node("preprocessor")->first_attribute("rgb")->value());
    colorSymbols = intValue(configNode->first_node("colors")->first_node("symbols")->first_attribute("rgb")->value());

    isValid = true;
}


Config::~Config() {
    if ( font ) qFreeFont(font);
}

void Config::RegisterFont() {
    if ( registerFont != "" ) {
        qRegisterFont(registerFont.c_str());
        font = qLoadFont(fontName.c_str(), fontSize, fontBold, fontItalic, fontUnderline);
	} else {
		font = qLoadFont(fontName.c_str(), fontSize, fontBold, fontItalic, fontUnderline);
	}
}
