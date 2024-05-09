// utilities.cc
#include "utilities.hh"

std::string msg;
LogLevel lv = INFO;

void myPrint(LogLevel level, const std::string& message) {
    if (level == DEBUG) {
        std::cout << message << std::endl;
    }
}