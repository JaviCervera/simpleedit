#ifndef GUILOADER_H
#define GUILOADER_H

#include <map>
#include <string>

class GuiLoader {
public:
    static std::map<std::string, void*> Load(std::string filename, void* externalParent);
protected:
    GuiLoader() {}
};

#endif // GUILOADER_H
