// utilities.cc
#include "utilities.hh"

std::string My_msg;
LogLevel lv = DEBUG;

void myPrint(LogLevel level, const std::string& message) {
    if (level == DEBUG) {
        std::cout << message << std::endl;
    }
}

