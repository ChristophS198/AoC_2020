#include <unordered_map>
#include <string>
#include <algorithm>

#include "../utility.h"

constexpr int TARGET_VAL{ 2020 };

int sol_1_1(const std::string &file_path)
{
    std::unordered_map<int, int> hash_map;

    std::vector<int> data = read_numbers_from_file<int>(file_path);

    for (auto first_val : data)
    {
        hash_map[first_val] = 1;
        if (auto search = hash_map.find(TARGET_VAL-first_val); search != hash_map.end())
        {
            return first_val*search->first;
        }
    }
    return 0;
}


int sol_1_2(const std::string &file_path)
{
    std::unordered_map<int, int> hash_map;

    std::vector<int> data = read_numbers_from_file<int>(file_path);
    std::sort(data.begin(), data.end());

    for (auto d : data)
    {
        hash_map[d] = 1;
    }

    for (size_t i=0; i<data.size(); ++i)
    {
        auto first_val{ data[i] };
        auto second_val_it{ data.begin()+i+1 };
        auto third_val = TARGET_VAL - first_val - *second_val_it; // third_val is the required value in order to sum up to TARGET_VAL 

        // iterate over all second_val_it as long as their sum is smaller than TARGET_VAL
        while (third_val > 0 && second_val_it<data.end())
        {
            if (auto search = hash_map.find(third_val); search != hash_map.end())
            {
                return first_val * search->first * *second_val_it;
            }
            ++second_val_it;
            third_val = TARGET_VAL - first_val - *second_val_it;
        }
    }

    return 0;
}

