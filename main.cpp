#include <iostream>
#include "1/sol_1.cpp"

int main(int, char**){
    std::cout << "Hello, from AoC_2020!\n";

    std::cout << sol_1_1("./1/data.txt") << std::endl;
    std::cout << sol_1_1("/home/christoph/Dokumente/VSC/C++/AoC_2020/1/data.txt") << std::endl;
    return 0;
}
