#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>


/**
 * @brief converts a vector of chars to a number
 * 
 * @param c_vec vector of chars
 * @return T number contained in char vector
 */
template<typename T>
T convert_to_num(const std::vector<char> c_vec)
{
    T num{};
    for (const auto& c : c_vec)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw std::invalid_argument("Trying to convert non-digit char to digit!");
        }
        num = num*10 + c - '0';
    }
    return num;
}

/**
 * @brief Converts a string containig multiple numbers to a vector of numbers (currently only positive numbers)
 * 
 * @param in_str string containing multiple numbers
 * @return std::vector<unsigned int> vector where all numbers contained in in_str are inserted
 */
template<typename T>
std::vector<T> parse_string_to_number_vec(const std::string& in_str)
{
    std::vector<T> number_vec{};
    std::vector<char> c_vec{};
    int sign{1}; // is set to -1 if a negative sign '-' is read at the first position
    for (const auto& c : in_str)
    {
        if (c_vec.empty() && c == '-')
        {
            sign = -1;
            continue;
        }
        if (c_vec.empty() && c == '+')
        {
            sign = 1;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            c_vec.push_back(c);
        }
        else
        {
            if (c_vec.size() > 0u)
            {
                T val = convert_to_num<T>(c_vec);
                number_vec.push_back(sign*val);
            }
            sign = 1;
            c_vec.clear();
        }
    }
    // Convert and insert last number
    if(c_vec.size() > 0)
    {
        number_vec.push_back(sign*convert_to_num<T>(c_vec));
        c_vec.clear();
    }
    return number_vec;
}

/**
 * @brief Reads numbers from a file and interprets each non-consecutive digit as a separate number
 * Each number is appended to the output vector (one-dimensional)
 * 
 * @tparam T 
 * @param file_path 
 * @return std::vector<T> 
 */
template<typename T>
std::vector<T> read_numbers_from_file(const std::string& file_path)
{
    std::vector<T> number_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            std::vector<T> num_in_line = parse_string_to_number_vec<T>(input_line);
            number_vec.insert(number_vec.end(),num_in_line.begin(), num_in_line.end());
        }
        input_file.close();   //close the file object.
    }
    return number_vec;
}