#include <string>
#include <vector>

#include "../utility.h"

using Label = std::uint32_t;


// Idea is to use a vector where the index corresponds to the cup label and the value
// at each index position denotes the succeddings cup label
// for each shuffling only three vector entries have to be manipulated
class CrabCups
{
public:
    CrabCups(const std::string &start_layout);
    CrabCups(const std::string &start_layout, Label upper_lim);

    void do_single_move();
    std::ostream &print_cups(std::ostream &out) const;
    std::string get_cups_after_1() const;
    std::uint64_t mult_cups_after_1() const;

private:
    std::vector<Label> cup_vec; // vector index corresponds to the cup label and the value to its successor
    Label cur_cup{ 0 }; // label of the current cup
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

std::uint64_t sol_23_2(const std::string &file_path)
{
    std::string layout = get_start_layout(file_path);

    CrabCups crab_cups_game(layout, 1'000'000);
    for (std::uint64_t i=0; i<10'000'000ull; ++i)
    {
        crab_cups_game.do_single_move();
    }
    return crab_cups_game.mult_cups_after_1();
}

void CrabCups::do_single_move()
{
    // 1. remove the three cups following cur_cup
    Label picked_cup_1 = cup_vec[cur_cup];

    // 2. close circle so cur_cup preceeds the fourth cup
    cup_vec[cur_cup] = cup_vec[cup_vec[cup_vec[picked_cup_1]]];

    // 3. select destination cup (label is cur_cups label -1)
    auto dest_label{ cur_cup==0 ? h_limit : cur_cup-1 };
    if (dest_label < l_limit) dest_label = h_limit;
    while (dest_label==picked_cup_1 || dest_label==cup_vec[picked_cup_1] 
            || dest_label == cup_vec[cup_vec[picked_cup_1]])
    {
        if (dest_label == 0) dest_label = h_limit;
        else --dest_label;
    }

    // 4. place picked cups clockwise after the destination cup
    Label old_succ = cup_vec[dest_label];
    cup_vec[dest_label] = picked_cup_1;
    cup_vec[cup_vec[cup_vec[picked_cup_1]]] = old_succ;

    // 5. select a new current cup
    cur_cup = cup_vec[cur_cup];
}

CrabCups::CrabCups(const std::string &start_layout, Label upper_lim)
{
    if (start_layout.length() == 0) return;

    cup_vec.resize( upper_lim );
    Label first_cup_label = start_layout.at(0) - '1';
    cur_cup = first_cup_label;
    l_limit = first_cup_label;
    h_limit = l_limit;

    for (size_t i=1; i<start_layout.length(); ++i)
    {
        Label succ_label = start_layout.at(i) - '1';
        cup_vec[cur_cup] = succ_label;
        cur_cup = succ_label;
        if (cur_cup < l_limit) l_limit = cur_cup;
        if (cur_cup > h_limit) h_limit = cur_cup;
    }
    cup_vec[cur_cup] = h_limit+1;
    for (Label l=h_limit+1; l<upper_lim; ++l)
    {
        cup_vec[l] = l+1;
    }
    h_limit = upper_lim-1;
    // complete the cycle by letting the last element point to the first cup
    cup_vec.back() = first_cup_label;
    cur_cup = first_cup_label;
}

CrabCups::CrabCups(const std::string &start_layout)
{
    if (start_layout.length() == 0) return;

    cup_vec.resize(start_layout.length());
    Label first_cup_label = start_layout.at(0) - '1'; // subtract 1 so internal indexing start with 0, alhtough cup labels start with 1
    cur_cup = first_cup_label;
    l_limit = first_cup_label;
    h_limit = l_limit;

    for (size_t i=1; i<start_layout.length(); ++i)
    {
        Label succ_label = start_layout.at(i) - '1'; // subtract 1 so internal indexing start with 0, alhtough cup labels start with 1
        cup_vec[cur_cup] = succ_label;
        cur_cup = succ_label;
        if (cur_cup < l_limit) l_limit = cur_cup;
        if (cur_cup > h_limit) h_limit = cur_cup;
    }
    // complete the cycle by letting the last element point to the first cup
    cup_vec[cur_cup] = first_cup_label;
    cur_cup = first_cup_label;
}

std::ostream &CrabCups::print_cups(std::ostream &out) const
{
    Label cup_it = cup_vec[cur_cup];
    out << cur_cup+1;
    while (cup_it != cur_cup)
    {
        out << " " << cup_it+1;
        cup_it = cup_vec[cup_it];
    }
    out << "\n";
}

std::uint64_t CrabCups::mult_cups_after_1() const
{
    Label cup_it = cup_vec.at(0);   
    return (static_cast<uint64_t>(cup_it+1)) * (cup_vec[cup_it]+1);
}


std::string CrabCups::get_cups_after_1() const
{
    std::string res;
    Label cup_it = cup_vec.at(0);
    while (cup_it != 0)
    {
        res += std::to_string(cup_it+1);
        cup_it = cup_vec[cup_it];
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