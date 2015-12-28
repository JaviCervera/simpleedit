#ifndef LOG_H
#define LOG_H

#include <string>

class Log {
public:
    static void Init();
    static void Write(const std::string& message);
protected:
    Log() {}
};

#endif // LOG_H
