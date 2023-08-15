#include <iostream>
#include "1/sol_1.cpp"
#include "2/sol_2.cpp"
#include "3/sol_3.cpp"
#include "4/sol_4.cpp"
#include "5/sol_5.cpp"
#include "6/sol_6.cpp"
#include "7/sol_7.cpp"
#include "8/sol_8.cpp"
#include "9/sol_9.cpp"
#include "10/sol_10.cpp"
#include "11/sol_11.cpp"
#include <filesystem>

std::string get_input_file_name(int day)
{
    return "../" + std::to_string(day) + "/data.txt";
}

int main(int /*argc*/, char** /*argv*/)
{
/*
    std::cout << sol_1_1(get_input_file_name(1)) << std::endl;
    std::cout << sol_1_2(get_input_file_name(1)) << std::endl;
    std::cout << sol_2_1(get_input_file_name(2)) << std::endl;
    std::cout << sol_2_2(get_input_file_name(2)) << std::endl;
    std::cout << sol_3_1(get_input_file_name(3)) << std::endl;
    std::cout << sol_3_2(get_input_file_name(3)) << std::endl;
    std::cout << sol_4_1(get_input_file_name(4)) << std::endl;
    std::cout << sol_4_2(get_input_file_name(4)) << std::endl;
    std::cout << sol_5_1(get_input_file_name(5)) << std::endl;
    std::cout << sol_5_2(get_input_file_name(5)) << std::endl;
    std::cout << sol_6_1(get_input_file_name(6)) << std::endl;
    std::cout << sol_6_2(get_input_file_name(6)) << std::endl;
    std::cout << sol_7_1(get_input_file_name(7)) << std::endl;
    std::cout << sol_7_2(get_input_file_name(7)) << std::endl;
    std::cout << sol_8_1(get_input_file_name(8)) << std::endl;
    std::cout << sol_8_2(get_input_file_name(8)) << std::endl;
    std::cout << sol_9_1(get_input_file_name(9)) << std::endl;
    std::cout << sol_9_2(get_input_file_name(9)) << std::endl;
    std::cout << sol_10_1(get_input_file_name(10)) << std::endl;
    std::cout << sol_10_2(get_input_file_name(10)) << std::endl;
*/
    std::cout << sol_11_1(get_input_file_name(11)) << std::endl;
    std::cout << sol_11_2(get_input_file_name(11)) << std::endl;

    return 0;
}
