#include <unordered_map>
#include <string>
#include <vector>

#include "../utility.h"

using GroupAnswer = std::vector<std::string>;

constexpr int NUM_ANSWERS{ 26 };

std::vector<GroupAnswer> read_answers_from_file(const std::string &file_path);
int get_num_any_yes(const GroupAnswer &group_answer);
int get_num_every_yes(const GroupAnswer &group_answer);

int sol_6_1(const std::string &file_path)
{
    std::vector<GroupAnswer> group_answers = read_answers_from_file(file_path);
    int sum{ 0 };

    for (const auto & answer : group_answers)
    {
        sum += get_num_any_yes(answer);
    }

    return sum;
}


int sol_6_2(const std::string &file_path)
{
    std::vector<GroupAnswer> group_answers = read_answers_from_file(file_path);
    int sum{ 0 };

    for (const auto & answer : group_answers)
    {
        sum += get_num_every_yes(answer);
    }

    return sum;
}

// either implement with unordered_map/set or vec of 26 elements
int get_num_any_yes(const GroupAnswer &group_answer)
{
    // std::vector<int> answer_vec(NUM_ANSWERS,0);
    int answer_vec[NUM_ANSWERS] = { 0 }; // all elements 0
    int num_yes{ 0 };

    for (const auto &answer_string : group_answer)
    {
        for (const auto &answer : answer_string)
        {
            auto &a = answer_vec[answer-'a'];
            if (0 == a)
            {
                a = 1;
                ++num_yes;
            }
        }
    }

    return num_yes;
}

// either implement with unordered_map/set or vec of 26 elements
int get_num_every_yes(const GroupAnswer &group_answer)
{
    int answer_vec[NUM_ANSWERS] = { 0 }; // all elements 0
    int answer_len{ group_answer.size() };

    for (int i=0; i<answer_len-1; ++i)
    {
        auto &answer_string = group_answer[i];
        for (const auto &answer : answer_string)
        {
            ++answer_vec[answer-'a'];
        }
    }

    int num_yes{ 0 };
    // iterate through last answer and count how many elements match answer_len-1
    auto &answer_string = group_answer[answer_len-1];
    for (const auto &answer : answer_string)
    {
        if (answer_len-1 == answer_vec[answer-'a']) ++num_yes;
    }
    return num_yes;
}

std::vector<GroupAnswer> read_answers_from_file(const std::string &file_path)
{
    std::vector<GroupAnswer> answer_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        GroupAnswer answer;
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            if (input_line == "")
            {
                answer_vec.push_back(answer);
                answer.clear();
            }
            else
            {
                answer.push_back(input_line);
            }
        }
        if (!answer.empty()) answer_vec.push_back(answer);
        input_file.close();   //close the file object.
    }
    return answer_vec;
}

