#include <unordered_map>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>

#include "../utility.h"

/*
Just for fun:
Implemented using a graph
*/

const std::string MY_BAG_NAME{ "shiny gold" };

struct TmpBag
{
    std::string bag_name;
    std::vector<std::pair<int,std::string>> contained_bags;
};

struct Bag
{
    std::vector<std::pair<int,Bag *>> contained_bags;
    std::vector<Bag *> parent_bags;
};

std::unordered_map<std::string,Bag *> get_input_graph(const std::string &file_path);

int sol_7_1(const std::string &file_path)
{
    std::unordered_map<std::string,Bag *> bag_map = get_input_graph(file_path);

    int sum{ -1 };
    std::unordered_set<Bag *> bag_set;
    std::stack<Bag *> bag_stack;
    bag_stack.push(bag_map[MY_BAG_NAME]);
    while (!bag_stack.empty())
    {
        auto cur = bag_stack.top();
        bag_stack.pop();
        if (bag_set.find(cur) != bag_set.end()) continue;

        ++sum;
        bag_set.insert(cur);
        for (auto &elem : cur->parent_bags)
        {
            bag_stack.push(elem);
        }
    }

    return sum;
}


int sol_7_2(const std::string &file_path)
{
    std::unordered_map<std::string,Bag *> bag_map = get_input_graph(file_path);
    int sum{ 0 };
    std::stack<std::pair<int,Bag *>> bag_stack; // stack with number of bags of this type and pointer to this bag
    bag_stack.push({ 1, bag_map[MY_BAG_NAME] });

    while (!bag_stack.empty())
    {
        auto cur = bag_stack.top();
        auto cur_bag = cur.second;
        auto cur_bag_count = cur.first;
        bag_stack.pop();

        for (auto &elem : cur_bag->contained_bags)
        {
            sum += cur_bag_count*elem.first;
            bag_stack.push({ cur_bag_count*elem.first,elem.second });
        }
    }

    return sum;
}

void parse_line(std::unordered_map<std::string,Bag *> &bag_map, const std::string &bag_string)
{
    std::vector<std::string> bag_string_split = split_string(bag_string, " bags contain ");
    std::string bag_name = bag_string_split[0];
    auto cur_bag_it = bag_map.find(bag_name);
    if (cur_bag_it == bag_map.end())
    {
        bag_map[bag_name] = new Bag();
    }
    auto &cur_bag = bag_map[bag_name];

    if ("no other bags" == bag_string_split[1]) return;
    else
    {
        std::vector<std::string> child_bags_string_split = split_string(bag_string_split[1], ", ");

        for (auto &child_bag : child_bags_string_split)
        {
            // child_bag has syntax "2 wavy purple bags"
            std::vector<std::string> child_string_split = split_string(child_bag, " ");
            if (4 != child_string_split.size()) throw std::runtime_error("Wrong assumption on bag naming convention!\n" + child_bag);
            int num = std::stoi(child_string_split[0]);
            std::string child_bag_name = child_string_split[1] + " " + child_string_split[2];

            auto child_bag_it = bag_map.find(child_bag_name);
            if (child_bag_it == bag_map.end())
            {
                bag_map[child_bag_name] = new Bag();
            }
            auto &cur_child_bag = bag_map[child_bag_name];
            cur_bag->contained_bags.push_back({ num, cur_child_bag});
            cur_child_bag->parent_bags.push_back(cur_bag);
        }
    }
}

std::unordered_map<std::string,Bag *> get_input_graph(const std::string &file_path)
{
    std::unordered_map<std::string,Bag *> bag_map;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            input_line.pop_back(); // remove dot from line
            parse_line(bag_map, input_line);
        }
        input_file.close();   
    }
    return bag_map;
}