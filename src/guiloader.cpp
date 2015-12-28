#include "guiloader.h"
#include "qtiny/qtiny.h"
#include "rapidxml.hpp"
#include "stringutils.hpp"

using namespace rapidxml;
using namespace stringutils;

struct Coords {
  int x;
  int y;
  int w;
  int h;
};

Coords ParseCoords(xml_node<>* node) {
  Coords coords;
  coords.x = intValue(node->first_attribute("x")->value());
  coords.y = intValue(node->first_attribute("y")->value());
  coords.w = intValue(node->first_attribute("width")->value());
  coords.h = intValue(node->first_attribute("height")->value());
  return coords;
}

void* GetParent(xml_node<>* node, std::map<std::string, void*>& widgets, void* externalParent) {
  if ( !node->first_attribute("parent") )
    return NULL;
  else if ( std::string(node->first_attribute("parent")->value()) == "external" )
    return externalParent;
  else
    return widgets[node->first_attribute("parent")->value()];
}

void* LoadWindow(xml_node<>* node, void* parent) {
  // Map for window flags
  std::map<std::string, int> windowFlags;
  windowFlags["tool"] = QWINDOW_TOOL;
  windowFlags["modal"] = QWINDOW_MODAL;
  windowFlags["sheet"] = QWINDOW_SHEET;
  windowFlags["centered"] = QWINDOW_CENTERED;

  // Parse params
  std::string title = node->first_node("title")->value();
  Coords coords = ParseCoords(node->first_node("coords"));
  int flags = 0;
  xml_node<>* flagNode = node->first_node("flag");
  while ( flagNode ) {
    flags |= windowFlags[flagNode->value()];
    flagNode = flagNode->next_sibling("flag");
  }

  // Create window
  return qCreateWindow(parent, title.c_str(), coords.x, coords.y, coords.w, coords.h, flags);
}

void* LoadMenu(xml_node<>* node, void* win) {
  void* menubar = qCreateMenuBar(win);

  xml_node<>* menuTitleNode = node->first_node("title");
  while ( menuTitleNode ) {
    void* menu = qAddMenuTitle(menubar, menuTitleNode->first_attribute("caption")->value());
    xml_node<>* menuItemNode = menuTitleNode->first_node();
    while ( menuItemNode ) {
      if ( std::string(menuItemNode->name()) == "item" ) {
        std::string caption = menuItemNode->first_attribute("caption")->value();
        std::string shortcut = "";
        if ( menuItemNode->first_attribute("shortcut") )
          shortcut += menuItemNode->first_attribute("shortcut")->value();
        qAddMenuItem(menu, intValue(menuItemNode->first_attribute("id")->value()), caption.c_str(), shortcut.c_str());
      } else if ( std::string(menuItemNode->name()) == "separator" ) {
        qAddMenuSeparator(menu);
      }
      menuItemNode = menuItemNode->next_sibling();
    }
    menuTitleNode = menuTitleNode->next_sibling("title");
  }

  return menubar;
}

void* LoadToolBar(xml_node<>* node, void* win, const std::string& filepath) {
  void* toolbar = qCreateToolBar(win);

  std::string path = "";
  if ( node->first_node("imagepath") )
    path = filepath + node->first_node("imagepath")->value();
  else
    path = filepath;

  xml_node<>* toolbarItemNode = node->first_node();
  while ( toolbarItemNode ) {
    if ( std::string(toolbarItemNode->name()) == "item" ) {
      int id = intValue(toolbarItemNode->first_attribute("id")->value());
      void* icon = qLoadIcon((path + toolbarItemNode->first_attribute("image")->value()).c_str());
      std::string tooltip = "";
      if ( toolbarItemNode->first_attribute("tooltip") )
        tooltip = toolbarItemNode->first_attribute("tooltip")->value();
      qAddToolBarButton(toolbar, id, icon, tooltip.c_str());
    } else if ( std::string(toolbarItemNode->name()) == "separator" ) {
      qAddToolBarSeparator(toolbar);
    }
    toolbarItemNode = toolbarItemNode->next_sibling();
  }

  return toolbar;
}

void* LoadStatusBar(xml_node<>* node, void* win) {
  void* statusbar = qCreateStatusBar(win);

  /*xml_node<>* fieldNode = node->first_node("field");
    while ( fieldNode ) {
        PB::AddStatusBarField(intValue(fieldNode->first_attribute("width")->value()));
        fieldNode = fieldNode->next_sibling("field");
    }*/

  return statusbar;
}

void* LoadTabBar(xml_node<>* node, void* parent) {
  return qCreateTabBar(parent);
}

void* LoadEditor(xml_node<>* node, void* parent) {
  BOOL readonly = std::string(node->first_attribute("readonly")->value()) == "true";
  BOOL highlighting = std::string(node->first_attribute("highlighting")->value()) == "true";
  Coords coords = ParseCoords(node->first_node("coords"));
  void* editor = qCreateEditor(parent, readonly, highlighting);
  qLayoutWidget(editor, coords.x, coords.y, coords.w, coords.h);
  return editor;
}

void* LoadButton(xml_node<>* node, void* parent) {
  std::string text = node->first_attribute("text")->value();
  Coords coords = ParseCoords(node->first_node("coords"));
  void* button = qCreateButton(parent, text.c_str());
  qLayoutWidget(button, coords.x, coords.y, coords.w, coords.h);
  return button;
}

void* LoadLabel(xml_node<>* node, void* parent) {
  std::string text = node->first_attribute("text")->value();
  Coords coords = ParseCoords(node->first_node("coords"));
  void* label = qCreateLabel(parent, text.c_str());
  qLayoutWidget(label, coords.x, coords.y, coords.w, coords.h);
  return label;
}

void* LoadLineEdit(xml_node<>* node, void* parent) {
  std::string text = "";
  if ( node->first_attribute("text") ) text = node->first_attribute("text")->value();
  Coords coords = ParseCoords(node->first_node("coords"));
  void* lineedit = qCreateLineEdit(parent, text.c_str());
  qLayoutWidget(lineedit, coords.x, coords.y, coords.w, coords.h);
  return lineedit;
}

void* LoadCheckBox(xml_node<>* node, void* parent) {
  std::string text = node->first_attribute("text")->value();
  Coords coords = ParseCoords(node->first_node("coords"));
  void* checkbox = qCreateCheckBox(parent, text.c_str());
  qLayoutWidget(checkbox, coords.x, coords.y, coords.w, coords.h);
  return checkbox;
}

void* LoadFrame(xml_node<>* node, void* parent) {
  // Map for frame flags
  std::map<std::string, int> frameFlags;
  frameFlags["noborder"] = QFRAME_NOBORDER;
  frameFlags["box"] = QFRAME_BOX;
  frameFlags["panel"] = QFRAME_PANEL;
  frameFlags["styled"] = QFRAME_STYLED;
  frameFlags["plain"] = QFRAME_PLAIN;
  frameFlags["raised"] = QFRAME_RAISED;
  frameFlags["sunken"] = QFRAME_SUNKEN;

  // Parse params
  std::string title = node->first_node("title")->value();
  Coords coords = ParseCoords(node->first_node("coords"));
  int flags = 0;
  xml_node<>* flagNode = node->first_node("flag");
  while ( flagNode ) {
    flags |= frameFlags[flagNode->value()];
    flagNode = flagNode->next_sibling("flag");
  }

  // Create frame
  void* frame = qCreateFrame(parent, flags);
  qLayoutWidget(frame, coords.x, coords.y, coords.w, coords.h);
  return frame;
}

std::map<std::string, void*> GuiLoader::Load(std::string filename, void* externalParent) {
  std::map<std::string, void*> widgets;

  std::string xmlstr = read(filename);
  if ( xmlstr == "" ) return widgets;

  xml_document<> doc;
  doc.parse<0>((char*)xmlstr.c_str());
  xml_node<>* interfaceNode = doc.first_node("interface");
  if ( !interfaceNode ) return widgets;

  xml_node<>* currentNode = interfaceNode->first_node();
  while ( currentNode ) {
    if ( std::string(currentNode->name()) == "window" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadWindow(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "menu" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadMenu(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "toolbar" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadToolBar(currentNode, parent, stringutils::extractPath(filename));
    } else if ( std::string(currentNode->name()) == "statusbar" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadStatusBar(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "tabbar" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadTabBar(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "editor" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadEditor(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "button" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadButton(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "label" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadLabel(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "lineedit" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadLineEdit(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "checkbox" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadCheckBox(currentNode, parent);
    } else if ( std::string(currentNode->name()) == "frame" ) {
      void* parent = GetParent(currentNode, widgets, externalParent);
      widgets[currentNode->first_attribute("key")->value()] = LoadFrame(currentNode, parent);
    }

    currentNode = currentNode->next_sibling();
  }

  return widgets;
}
