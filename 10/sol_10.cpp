#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "../utility.h"

int get_num_combinations(int i);

int sol_10_1(const std::string &file_path)
{
    std::vector<int> jolt_numbers = read_numbers_from_file<int>(file_path);

    std::sort(jolt_numbers.begin(), jolt_numbers.end());

    int diffs[4] = { 0 }; 

    int last_jolt{ 0 };
    for (auto jolt : jolt_numbers)
    {
        ++diffs[jolt - last_jolt];
        last_jolt = jolt;
    }

    ++diffs[3]; // diff from device's built-in adapter

    return diffs[1] * diffs[3];
}

/*
Idea is that a number (node) can be reached by up to three predecessors -> we have to 
add up all the combinations of those (potential) 3 predecessors to get the number of 
combinations the current node can be reached
Initially we have 1 combination for 0 (which is added to the jolt_numbers)
*/
uint64_t sol_10_2(const std::string &file_path)
{
    std::vector<int> jolt_numbers = read_numbers_from_file<int>(file_path);
    jolt_numbers.push_back(0);
    std::sort(jolt_numbers.begin(), jolt_numbers.end());

    int max_val = jolt_numbers.back();
    uint64_t comb_vec[max_val+1] = { 0 }; // Init all elements to 0
    comb_vec[0] = 1;

    for (auto x : jolt_numbers) 
    {
        comb_vec[x+1] += comb_vec[x];
        comb_vec[x+2] += comb_vec[x];
        comb_vec[x+3] += comb_vec[x];
    }

    return comb_vec[max_val];
}

uint64_t sol_10_2_2(const std::string &file_path)
{
    std::vector<int> jolt_numbers = read_numbers_from_file<int>(file_path);

    std::sort(jolt_numbers.begin(), jolt_numbers.end());

    int diffs[4] = { 0 }; 
    int last_jolt{ 0 };
    uint64_t diff_1_ct{ 0 }; // counts number of contiguous 1 in diff
    uint64_t num_combs{ 1 };

    for (auto jolt : jolt_numbers)
    {
        ++diffs[jolt - last_jolt];
        if (1 == jolt-last_jolt) ++diff_1_ct;
        else
        {
            num_combs *= get_num_combinations(diff_1_ct);
            diff_1_ct = 0;
        }
        last_jolt = jolt;
    }
    ++diffs[3]; // diff from device's built-in adapter
    if (diff_1_ct > 0) num_combs *= get_num_combinations(diff_1_ct);

    return num_combs;
}


// returns the number of valid combinations for i contiguous 1s
int get_num_combinations(int i)
{
    static std::map<int,int> comb_map = { {0,1}, {1,1},{2,2},{3,4},{4,7} };
    return comb_map.at(i);
}