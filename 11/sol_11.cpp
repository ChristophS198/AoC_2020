#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "../utility.h"

constexpr char FLOOR{ '.' };
constexpr char EMPTY{ 'L' };
constexpr char OCCUPIED{ '#' };

using SeatLayout = std::vector<std::vector<char>>;

std::pair<SeatLayout,int> do_round(const SeatLayout &seat_layout, int neigh_thresh=3, bool is_task_2=false);
int num_occupied_neighbors(const SeatLayout &seat_layout, int row, int col);
int num_occupied_neighbors_2(const SeatLayout &seat_layout, int row, int col);
int seat_count(const SeatLayout &seat_layout);
bool only_empty_neighbors(const SeatLayout &seat_layout, int row, int col);
bool only_empty_neighbors_2(const SeatLayout &seat_layout, int row, int col);
SeatLayout read_seat_layout_from_file(const std::string& file_path);
void print_layout(std::ostream &out, const SeatLayout &seat_layout);

int sol_11_1(const std::string &file_path)
{
    SeatLayout seat_layout = read_seat_layout_from_file(file_path);
    
    std::pair<SeatLayout,int> res = do_round(seat_layout);

    while (0 < res.second) res = do_round(res.first);
    
    return seat_count(res.first);
}

int sol_11_2(const std::string &file_path)
{
    SeatLayout seat_layout = read_seat_layout_from_file(file_path);
    
    std::pair<SeatLayout,int> res = do_round(seat_layout, 4, true);
    // std::ofstream out_file("layout.txt");
    // print_layout(out_file, res.first);
    while (0 < res.second) 
    {
        res = do_round(res.first, 4, true);
        // print_layout(out_file, res.first);
    }
    
    return seat_count(res.first);
}

bool only_empty_neighbors(const SeatLayout &seat_layout, int row, int col)
{
    std::vector<std::pair<int,int>> neighbors = { {1,1},{1,0},{0,1},{-1,-1},
                                                   {-1,0},{0,-1},{1,-1}, {-1,1} };
    for (auto &neigh : neighbors)
    {
        if (OCCUPIED == seat_layout[row+neigh.first][col+neigh.second]) return false;
    }
    return true;
}
int num_occupied_neighbors(const SeatLayout &seat_layout, int row, int col)
{
    int count{ 0 };

    std::vector<std::pair<int,int>> neighbors = { {1,1},{1,0},{0,1},{-1,-1},
                                                   {-1,0},{0,-1},{1,-1}, {-1,1} };
    for (auto &neigh : neighbors)
    {
        if (OCCUPIED == seat_layout[row+neigh.first][col+neigh.second]) ++count;
    }
    return count;
}
bool only_empty_neighbors_2(const SeatLayout &seat_layout, int row, int col)
{
    int r_size = seat_layout.size()-1;
    int c_size = seat_layout[0].size()-1;
    std::vector<std::pair<int,int>> directions = { {1,1},{1,0},{0,1},{-1,-1},
                                                   {-1,0},{0,-1},{1,-1}, {-1,1} };
    for (auto &dir : directions)
    {
        int t_row = row+dir.first;
        int t_col = col+dir.second;
        while (t_row < r_size && t_col < c_size && t_row > 0 && t_col > 0 &&
            EMPTY != seat_layout[t_row][t_col])
        {
            if (OCCUPIED == seat_layout[t_row][t_col]) 
            {
                return false;
            }
            t_row += dir.first;
            t_col += dir.second;
        }
    }
    return true;
}
int num_occupied_neighbors_2(const SeatLayout &seat_layout, int row, int col)
{
    int count{ 0 };
    int r_size = seat_layout.size();
    int c_size = seat_layout[0].size();
    std::vector<std::pair<int,int>> directions = { {1,1},{1,0},{0,1},{-1,-1},
                                                   {-1,0},{0,-1},{1,-1}, {-1,1} };
    for (auto &dir : directions)
    {
        int t_row = row+dir.first;
        int t_col = col+dir.second;
        while (t_row < r_size && t_col < c_size && t_row > 0 && t_col > 0 &&
            EMPTY != seat_layout[t_row][t_col])
        {
            if (OCCUPIED == seat_layout[t_row][t_col]) 
            {
                ++count;
                break;
            }
            t_row += dir.first;
            t_col += dir.second;
        }
    }
    return count;
}

std::pair<SeatLayout,int> do_round(const SeatLayout &seat_layout, int neigh_thresh, bool is_task_2)
{
    SeatLayout new_layout{ seat_layout };
    int r_size = seat_layout.size()-1;
    int c_size = seat_layout[0].size()-1;
    int num_changes{ 0 };

    for (int row=1; row<r_size; ++row)
    {
        for (int col=1; col<c_size; ++col)
        {
            int num_occ_neigh{ 0 };
            auto cur_seat = seat_layout[row][col];
            switch (cur_seat)
            {
            case FLOOR:
                continue;
                break;
            case OCCUPIED:
                if (is_task_2) num_occ_neigh = num_occupied_neighbors_2(seat_layout,row,col);
                else num_occ_neigh = num_occupied_neighbors(seat_layout,row,col);
                if (neigh_thresh < num_occ_neigh) 
                {
                    ++num_changes;
                    new_layout[row][col] = EMPTY;
                }
                break;
            case EMPTY:
                if (!is_task_2 && only_empty_neighbors(seat_layout,row,col) ||
                    is_task_2 && only_empty_neighbors_2(seat_layout,row,col)) 
                {
                    ++num_changes;
                    new_layout[row][col] = OCCUPIED;
                }
                break;
            default:
                break;
            }
        }
    }
    return { new_layout,num_changes };
}

// return seat layout with Floor borders 
SeatLayout read_seat_layout_from_file(const std::string& file_path)
{
    SeatLayout char_vec_2d{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            if (char_vec_2d.empty()) char_vec_2d.push_back(std::vector<char>(input_line.length()+2,FLOOR)); 
            char_vec_2d.push_back({FLOOR});
            char_vec_2d.back().insert(char_vec_2d.back().end(),input_line.begin(), input_line.end());
            char_vec_2d.back().push_back(FLOOR);
        }
        char_vec_2d.push_back(std::vector<char>(input_line.length()+2,FLOOR)); 
        input_file.close();   //close the file object.
    }
    return char_vec_2d;
}

int seat_count(const SeatLayout &seat_layout)
{
    int count{ 0 };

    for (auto row : seat_layout)
    {
        for (auto elem : row)
        {
            if (elem == OCCUPIED) ++count;
        }
    }

    return count;
}

void print_layout(std::ostream &out, const SeatLayout &seat_layout)
{
    for (int row=1; row<seat_layout.size()-1; ++row)
    {
        for (int col=1;col<seat_layout[row].size()-1; ++col)
        {
            out << seat_layout[row][col];
        }
        out << "\n";
    }
    out << "\n" << std::flush;
}