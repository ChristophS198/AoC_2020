#include <unordered_map>
#include <string>

#include "../utility.h"

constexpr int TARGET_VAL{ 2020 };

int sol_1_1(const std::string &file_path)
{
    std::unordered_map<int, int> hash_map;

    std::vector<int> data = read_numbers_from_file<int>(file_path);

    for (auto d : data)
    {
        hash_map[d] = 1;
        if (auto search = hash_map.find(TARGET_VAL-d); search != hash_map.end())
        {
            return d*search->first;
        }
    }


    return 0;
}


