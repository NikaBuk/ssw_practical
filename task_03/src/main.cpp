#include <iostream>
#include <string>
#include "task_03.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#ifdef _WIN32
#include <windows.h>

void sleep(unsigned milliseconds) {
    Sleep(milliseconds);
}
#else
#include <unistd.h>

void sleep(unsigned milliseconds) {
    usleep(milliseconds * 1000); // takes microseconds
}
#endif

int main() {
    std::cout << "RUN..." << std::endl;

    for (int i = 1; i < 11 ; i++) {
        auto tmp_path = R"(C:\Users\IlyaN\Documents\GitHub\kkk\task_03\test)" +
                        std::to_string(i) + ".p";
        std::cout << "PARSE " + tmp_path << std::endl;
        Parse3(tmp_path);
        sleep(1000);
        std::cout << "END" << std::endl;
    }

    std::cout << "END OF TESTING" << std::endl;
    std::getchar();
}

#pragma clang diagnostic pop