#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../utility.h"

struct Cup;

using Label = int;
using CupPtr = std::shared_ptr<Cup>;

struct Cup
{
    Label val;
    CupPtr succ; // Pointer to next (clockwise) cup
};

class CrabCups
{
public:
    CrabCups(const std::string &start_layout);
    CrabCups(const std::string &start_layout, Label upper_lim);

    void do_single_move();
    std::ostream &print_cups(std::ostream &out) const;
    std::string get_cups_after_1()  const;

private:
    std::unordered_map<Label,CupPtr> cup_map; // Inside this map the cup objects are stored for fast access based on label
    CupPtr cur_cup = nullptr; // cur_map pointer is used to access the current element and iterate through cup list
    Label l_limit{ 0 };
    Label h_limit{ 0 };
};

std::string get_start_layout(const std::string& file_path);

std::string sol_23_1(const std::string &file_path)
{
    std::string layout = get_start_layout(file_path);

    CrabCups crab_cups_game(layout);
    for (int i=0; i<100; ++i)
    {
        crab_cups_game.do_single_move();
    }
    return crab_cups_game.get_cups_after_1();
}

int sol_23_2(const std::string &file_path)
{
    std::string layout = get_start_layout(file_path);

    CrabCups crab_cups_game(layout, 100);
    std::ofstream out("crab_game.txt");
    for (int i=0; i<100; ++i)
    {
        out << "Round " << i << ": ";
        crab_cups_game.print_cups(out);
        crab_cups_game.do_single_move();
    }
    out << "Round " << 100 << ": ";
    out.close();
    crab_cups_game.print_cups(out);
    return 0;
}

void CrabCups::do_single_move()
{
    // 1. remove the three cups following cur_cup
    CupPtr picked_cups = cur_cup->succ;

    // 2. close circle so cur_cup preceeds the fourth cup
    cur_cup->succ = cur_cup->succ->succ->succ->succ;

    // 3. select destination cup (label is cur_cups label -1)
    auto dest_label{ cur_cup->val-1 };
    if (dest_label < l_limit) dest_label = h_limit;
    while (dest_label==picked_cups->val || dest_label==picked_cups->succ->val 
            || dest_label == picked_cups->succ->succ->val)
    {
        --dest_label;
        if (dest_label < l_limit) dest_label = h_limit;
    }
    auto dest_cup = cup_map[dest_label];

    // 4. place picked cups clockwise after the destination cup
    CupPtr old_succ = dest_cup->succ;
    dest_cup->succ = picked_cups;
    picked_cups->succ->succ->succ = old_succ;

    // 5. select a new current cup
    cur_cup = cur_cup->succ;

}

CrabCups::CrabCups(const std::string &start_layout, Label upper_lim)
{
    if (start_layout.length() == 0) return;

    Label first_cup_label = start_layout.at(0) - '0';
    cup_map.emplace(first_cup_label,std::make_shared<Cup>( Cup{ first_cup_label, nullptr }));
    cur_cup = cup_map[first_cup_label];
    l_limit = first_cup_label;
    h_limit = l_limit;

    for (size_t i=1; i<start_layout.length(); ++i)
    {
        Label label = start_layout.at(i) - '0';
        cup_map.emplace(label,std::make_shared<Cup>( Cup{ label, nullptr }));
        cur_cup->succ = cup_map[label];
        cur_cup = cup_map[label];
        if (cur_cup->val < l_limit) l_limit = cur_cup->val;
        if (cur_cup->val > h_limit) h_limit = cur_cup->val;
    }

    for (Label l=h_limit+1; l<=upper_lim; ++l)
    {
        cup_map.emplace(l,std::make_shared<Cup>( Cup{ l, nullptr }));
        cur_cup->succ = cup_map[l];
        cur_cup = cup_map[l];
    }
    h_limit = upper_lim;
    // complete the cycle by letting the last element point to the first cup
    cur_cup->succ = cup_map[first_cup_label];
    cur_cup = cup_map[first_cup_label];
}

CrabCups::CrabCups(const std::string &start_layout)
{
    if (start_layout.length() == 0) return;

    Label first_cup_label = start_layout.at(0) - '0';
    cup_map.emplace(first_cup_label,std::make_shared<Cup>( Cup{ first_cup_label, nullptr }));
    cur_cup = cup_map[first_cup_label];
    l_limit = first_cup_label;
    h_limit = l_limit;

    for (size_t i=1; i<start_layout.length(); ++i)
    {
        Label label = start_layout.at(i) - '0';
        cup_map.emplace(label,std::make_shared<Cup>( Cup{ label, nullptr }));
        cur_cup->succ = cup_map[label];
        cur_cup = cup_map[label];
        if (cur_cup->val < l_limit) l_limit = cur_cup->val;
        if (cur_cup->val > h_limit) h_limit = cur_cup->val;
    }
    // complete the cycle by letting the last element point to the first cup
    cur_cup->succ = cup_map[first_cup_label];
    cur_cup = cup_map[first_cup_label];
}

std::ostream &CrabCups::print_cups(std::ostream &out) const
{
    CupPtr cup_it = cur_cup->succ;
    out << cur_cup->val;
    while (cup_it != cur_cup)
    {
        out << " " << cup_it->val;
        cup_it = cup_it->succ;
    }
    out << "\n";
}

std::string CrabCups::get_cups_after_1() const
{
    std::string res;
    CupPtr cup_it = cur_cup;
    while (cup_it->val != 1)
    {
        cup_it = cup_it->succ;
    }
    cup_it = cup_it->succ;
    while (cup_it->val != 1)
    {
        res += std::to_string(cup_it->val);
        cup_it = cup_it->succ;
    }
    return res;
}

std::string get_start_layout(const std::string& file_path)
{
    std::string start_layout{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        getline(input_file, start_layout);
        input_file.close();   //close the file object.
    }
    return start_layout;
}