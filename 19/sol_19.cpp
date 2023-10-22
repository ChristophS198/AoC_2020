#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <memory>

#include "../utility.h"

using RuleId = int;
using RuleStrMap = std::unordered_map<RuleId,std::string>;


std::pair<std::vector<std::string>,RuleStrMap> get_data_19(const std::string &file_path);
std::vector<size_t> rec_matcher(RuleId rule_id, std::string msg, size_t &pos, const RuleStrMap &rule_str_map);


int sol_19_1(const std::string &file_path)
{
    std::pair<std::vector<std::string>,RuleStrMap> data = get_data_19(file_path);
    size_t max_len{ 0 };
    for (const auto &msg : data.first) 
    {
        if (msg.length() > max_len) max_len = msg.length();
    }
    
    int match_count{ 0 };
    for (const auto &msg : data.first)
    {
        size_t pos{ 0 };
        std::vector<size_t> res = rec_matcher(0, msg, pos, data.second);
        if (!res.empty() && res[0] == msg.length()) 
        {
            ++match_count;
        }
    }

    return match_count;
}


int sol_19_2(const std::string &file_path)
{
    std::pair<std::vector<std::string>,RuleStrMap> data = get_data_19(file_path);
    data.second.at(11) = "42 31 | 42 11 31";
    data.second.at(8) = "42 | 42 8";

    size_t max_len{ 0 };
    for (const auto &msg : data.first) 
    {
        if (msg.length() > max_len) max_len = msg.length();
    }
    
    int match_count{ 0 };
    for (const auto &msg : data.first)
    {
        size_t pos{ 0 };
        std::vector<size_t> res = rec_matcher(0, msg, pos, data.second);
        if (!res.empty() && res[0] == msg.length()) 
        {
            ++match_count;
        }
    }

    return match_count;
}

std::pair<std::vector<std::string>,RuleStrMap> get_data_19(const std::string &file_path)
{
    std::vector<std::string> msg_vec{};
    RuleStrMap rule_str_map{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;

        // read Rules from file
        while(getline(input_file, input_line) && "" != input_line){
            std::vector<std::string> split_res = split_string(input_line,": ");
            RuleId rule_id = std::stoi(split_res.at(0));
            rule_str_map[rule_id] = split_res.at(1);
        }

        // read messages from file
        while(getline(input_file, input_line) && "" != input_line){
            msg_vec.push_back(input_line);
        }
        input_file.close();   
    }
    return std::make_pair(msg_vec,rule_str_map);
}


/*
Idea is to iterate through the rules and evaluate on the fly whether the rules are still fulfilled
In order to take recursive rules into account multiple correct positions can be returned (position 
corresponds to the index where the next rule starts to check letters). That is, whenever subrules
are connected through OR (|) multiple positions for the following AND subrule might be returned.
For example:
aaabbaabbaabba
if a rule 8: 42 | 8 with 42=abba
three valid positions would be returned, because abba does match, but abbaabba and abbaabbaabba do also match
This assures that all rec combinations are evaluated
*/
std::vector<size_t> rec_matcher(RuleId rule_id, std::string msg, size_t &pos, const RuleStrMap &rule_str_map)
{
    if (pos >= msg.length()) 
    {
        return { };
    }

    auto rule_str = rule_str_map.at(rule_id);

    // distignuish between rule_str containing subrules and rule_str with "a" or "b"
    if ('"' == rule_str[0])
    {
        if (msg.at(pos) == rule_str[1])
        {
            ++pos;
            return { pos };
        }
        else return {};
    }
    else
    {
        std::vector<std::string> sub_rules_or = split_string(rule_str," | ");
        std::vector<size_t> possible_pos;
        for (auto &sub_rule : sub_rules_or)
        {
            // iterate through each subrule and check whether each subsubrule is fulfilled (AND)
            std::vector<std::string> sub_rules_and = split_string(sub_rule," ");
            std::vector<size_t> new_psbl_positions = { pos };
            for (const auto &rule_id_str : sub_rules_and)
            {
                std::vector<size_t> res;
                auto rule_id_tmp = stoi(rule_id_str);
                for (auto pos_tmp : new_psbl_positions)
                {
                    auto res2 = rec_matcher(rule_id_tmp, msg, pos_tmp, rule_str_map);
                    if (!res2.empty()) 
                    {
                        res.insert(res.end(), res2.begin(), res2.end());
                    }
                }
                new_psbl_positions = res;
                if (res.empty()) break;
            }
            if (!new_psbl_positions.empty()) 
            {
                possible_pos.insert(possible_pos.end(), new_psbl_positions.begin(), new_psbl_positions.end());
            }
        }
        return possible_pos;
    }

    return { };
}
