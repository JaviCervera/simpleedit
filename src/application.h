#ifndef APPLICATION_H
#define APPLICATION_H

class Config;
class Controller;
class Language;

class Application {
public:
    static Application* Instance();
    int Run(int argc, char**argv);
    void SetController(Controller* controller, bool deleteOld);
    Controller* GetController() { return controller; }
    Config* GetConfig() { return config; }
    Language* GetLanguage() { return language; }
protected:
    Application() {}
    ~Application() {}
    static void EventHandler(int event, void* source, int id);
private:
    static Application* instance;
    Controller* controller;
    Config* config;
    Language* language;
};

#endif // APPLICATION_H
