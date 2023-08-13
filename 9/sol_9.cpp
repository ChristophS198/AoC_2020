#include <unordered_set>
#include <string>
#include <vector>

#include "../utility.h"

constexpr int PREAMBLE_LEN{ 25 };

std::pair<int,int> get_encryption_weakness(const std::vector<int> &nums, int target_val);
int get_first_out_of_place_val(const std::vector<int> &nums);
bool is_sum_of_2_in_set(std::unordered_set<int> &preamble, int target_val);

int sol_9_1(const std::string &file_path)
{
    std::vector<int> xmas_numbers = read_numbers_from_file<int>(file_path);

    return get_first_out_of_place_val(xmas_numbers);
}


int sol_9_2(const std::string &file_path)
{
    std::vector<int> xmas_numbers = read_numbers_from_file<int>(file_path);
    int target_val =  get_first_out_of_place_val(xmas_numbers);

    std::pair<int,int> range = get_encryption_weakness(xmas_numbers, target_val);
    int min_val{ xmas_numbers[range.first] };
    int max_val{ xmas_numbers[range.first] };

    for (int i=range.first; i<=range.second; ++i)
    {
        int cur_val = xmas_numbers[i];
        if (cur_val < min_val) min_val = cur_val;
        if (cur_val > max_val) max_val = cur_val;
    }

    return min_val + max_val;
}

std::pair<int,int> get_encryption_weakness(const std::vector<int> &nums, int target_val)
{
    int idx_low{ 0 };
    int idx_high{ 0 };
    int sum{ nums[0] };

    while (sum != target_val)
    {
        if (sum < target_val) sum += nums[++idx_high];
        if (sum > target_val) sum -= nums[idx_low++];
    }

    return { idx_low,idx_high };
}

int get_first_out_of_place_val(const std::vector<int> &nums)
{
    std::unordered_set<int> preamble_set;
    int idx{ 0 };

    while (idx < PREAMBLE_LEN) preamble_set.insert(nums[idx++]);

    while (is_sum_of_2_in_set(preamble_set, nums[idx]))
    {
        // Update set by removing oldest element and adding new one
        preamble_set.erase(nums[idx-PREAMBLE_LEN]);
        preamble_set.insert(nums[idx]);
        ++idx;
    }
    return nums[idx];
}

bool is_sum_of_2_in_set(std::unordered_set<int> &preamble, int target_val)
{
    for (auto &first_val : preamble)
    {
        int second_val = target_val - first_val;
        if (auto search=preamble.find(second_val); search != preamble.end()) return true;
    }

    return false;
}

