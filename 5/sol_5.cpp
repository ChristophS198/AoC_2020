#include <unordered_map>
#include <string>
#include <algorithm>

#include "../utility.h"

struct Interval
{
    int low;
    int high;
};

Interval binary_partion(Interval in, bool take_upper);
int partion_row(Interval in, std::string instructions);
int partion_col(Interval in, std::string instructions);
int get_sead_id(Interval row_intv, Interval col_intv, std::string instructions);
int get_free_seat_id(std::vector<int> &seat_id_vec);

int sol_5_1(const std::string &file_path)
{
    std::vector<std::string> boarding_passes = read_string_vec_from_file(file_path);
    Interval row_intv{0, 127};
    Interval col_intv{0, 7};
    int max_seat_id{ 0 };

    for (const auto & boarding_pass : boarding_passes)
    {
        int seat_id = get_sead_id(row_intv, col_intv, boarding_pass);
        max_seat_id = seat_id > max_seat_id ? seat_id : max_seat_id;
    }

    return max_seat_id;
}


int sol_5_2(const std::string &file_path)
{
    std::vector<std::string> boarding_passes = read_string_vec_from_file(file_path);
    Interval row_intv{0, 127};
    Interval col_intv{0, 7};
    std::vector<int> seat_id_vec;

    for (const auto & boarding_pass : boarding_passes)
    {
        int seat_id = get_sead_id(row_intv, col_intv, boarding_pass);
        seat_id_vec.push_back(seat_id);
    }

    return get_free_seat_id(seat_id_vec);
}

int get_free_seat_id(std::vector<int> &seat_id_vec)
{
    std::sort(seat_id_vec.begin(), seat_id_vec.end());

    int pred{ 0 };
    for (size_t i=1; i<seat_id_vec.size(); ++i)
    {
        if (seat_id_vec[i]-seat_id_vec[i-1] == 2) return seat_id_vec[i]-1; 
    }

    return -1;
}

Interval binary_partion(Interval in, bool take_upper)
{
    int len{ in.high-in.low+1 };
    if (take_upper)
    {
        in.low += len/2;
    }
    else
    {
        in.high -= len/2;
    }
    return in;
}

int partion_row(Interval in, std::string instructions)
{
    for (auto c : instructions)
    {
        switch (c)
        {
        case 'B':
            in = binary_partion(in,true);
            break;
        case 'F':
            in = binary_partion(in,false);
            break;
        default:
            throw std::invalid_argument("Invalid row partion arg: " + c);
            break;
        }
    }
    if (in.low != in.high) throw std::runtime_error("Failed to calc row!");
    return in.low;
}

int partion_col(Interval in, std::string instructions)
{

    for (auto c : instructions)
    {
        switch (c)
        {
        case 'R':
            in = binary_partion(in,true);
            break;
        case 'L':
            in = binary_partion(in,false);
            break;
        default:
            throw std::invalid_argument("Invalid col partion arg: " + c);
            break;
        }
    }
    if (in.low != in.high) throw std::runtime_error("Failed to calc col!");
    return in.low;
}

int get_sead_id(Interval row_intv, Interval col_intv, std::string instructions)
{
    int row = partion_row(row_intv, instructions.substr(0,7));
    int col = partion_col(col_intv, instructions.substr(7));
    return row*8 + col;
}