#include "application.h"

int main(int argc, char* argv[]) {
    Application::Instance()->Run(argc, argv);
    return 0;
}
