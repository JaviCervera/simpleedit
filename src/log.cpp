#include "log.h"
#include "stringutils.hpp"
#include <iostream>

void Log::Init() {
#ifdef USE_LOG
    stringutils::write("log.txt", "", false);
#endif
}

void Log::Write(const std::string& message) {
    std::cout << message.c_str() << std::endl;
#ifdef USE_LOG
    stringutils::write("log.txt", message + "\n", true);
#endif
}
