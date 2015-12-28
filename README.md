## SimpleEdit
A simple Integrated Development Environment based on Qt, and released under the terms of the GNU/GPL v3 license (see LICENSE file). It be easily modified through configuration scripts in XML format to support compiling and highlighting for custom programming languages.

To build SimpleEdit, you must open ***simpleedit.pro*** with **Qt Creator** (tested on version 3.5.1 with Qt 5.5.1). Within that file, you'll find the **TARGET** option on first line. You can change the value of this options to support different languages. For example, if you use *TARGET=LuaEdit*, the files *LuaEdit.config.xml* and *LuaEdit.language.xml* must be present within the *_build/cfg* folder. The first one is used to configure windows title, editor colors, file extensions, tab size, etc. The second one is used to provide the regular expressions to perform syntax highlighting for the language.

Apart from this, three additional config files exist in the *_build/cfg* folder: *compile.xml*, *find.xml*, *mainwindow.xml*. These files are used to describe the user interface for the compiler and find dialogs and the main window, respectively.

Lastly, there are two compiler macros that you can enable in *simpleedit.pro*:

* **USE_LOG**: If this is defined, every time the program runs, a *log.txt* file is generated that logs program execution.
* **USE_COMPILE_CONTROL**: If this is defined, a GUI dialog will be used to show compiler output. If it is not defined, a standard command line terminal will be opened.
