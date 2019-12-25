#include <iostream>
#include <regex>
#include <map>
#include <string>
#include <conio.h>
#include "task_03.h"
#include "time.h"

int main() {
    std::cout << "RUN..." << std::endl;
    for(int i = 1; i < 11 ; i++) {
        auto tmp_path = "C:\\Users\\IlyaN\\Documents\\GitHub\\kkk\\task_03\\test"+std::to_string(i)+".p";
        std::cout<<"PARSE " + tmp_path <<std::endl;
        Parse3(tmp_path);
        std::cout << "END" << std::endl;
        std::getchar();
    }
    std::cout << "END OF TESTING" << std::endl;
    std::getchar();
}
