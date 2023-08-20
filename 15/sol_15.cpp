#include <string>
#include <vector>
#include <unordered_map>

#include "../utility.h"

using tNumType = uint32_t;
using tRound = uint32_t;

constexpr tRound TARGET_ROUND{ 2020u };
constexpr tRound TARGET_ROUND_2{ 30'000'000u };


tNumType sol_15_1(const std::string &file_path)
{
    std::vector<tNumType> start_numbers = read_numbers_from_file<tNumType>(file_path);
    std::unordered_map<tNumType,tRound> num_round_map;
    tRound round{ 0 };
    tNumType latest_num{ 0 };
    tRound last_num_diff{ 0 };

    for (auto start_num : start_numbers)
    {
        if (auto search = num_round_map.find(start_num); search != num_round_map.end())
        {
            last_num_diff = round - search->second;
            search->second = round;
        }
        else
        {
            num_round_map[start_num] = round;
            last_num_diff = 0;
        }
        latest_num = start_num;
        ++round;
    }

    while (round < TARGET_ROUND)
    {
        if (last_num_diff == 0)  // denotes the case that a new number had been said in last round
        {
            latest_num = 0;
        }
        else
        {
            latest_num = last_num_diff;
        }
        if (auto search = num_round_map.find(latest_num); search != num_round_map.end())
        {
            last_num_diff = round - search->second;
            search->second = round;
        }
        else
        {
            num_round_map[latest_num] = round;
            last_num_diff = 0;
        }
        ++round;
    }

    return latest_num;
}


tNumType sol_15_2(const std::string &file_path)
{
std::vector<tNumType> start_numbers = read_numbers_from_file<tNumType>(file_path);
    std::unordered_map<tNumType,tRound> num_round_map;
    tRound round{ 0 };
    tNumType latest_num{ 0 };
    tRound last_num_diff{ 0 };

    for (auto start_num : start_numbers)
    {
        if (auto search = num_round_map.find(start_num); search != num_round_map.end())
        {
            last_num_diff = round - search->second;
            search->second = round;
        }
        else
        {
            num_round_map[start_num] = round;
            last_num_diff = 0;
        }
        latest_num = start_num;
        ++round;
    }

    while (round < TARGET_ROUND_2)
    {
        if (last_num_diff == 0)  // denotes the case that a new number had been said in last round
        {
            latest_num = 0;
        }
        else
        {
            latest_num = last_num_diff;
        }
        if (auto search = num_round_map.find(latest_num); search != num_round_map.end())
        {
            last_num_diff = round - search->second;
            search->second = round;
        }
        else
        {
            num_round_map[latest_num] = round;
            last_num_diff = 0;
        }
        ++round;
    }

    return latest_num;
}
