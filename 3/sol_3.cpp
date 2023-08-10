#include <vector>
#include <string>

#include "../utility.h"

constexpr char OPEN_SQUARE{ '.' };
constexpr char TREE{ '#' };


int count_tree_hits(const std::vector<std::vector<char>> &map, int step_size_right=3, int step_size_down=1);

int sol_3_1(const std::string &file_path)
{
    std::vector<std::vector<char>> map = read_2d_vec_from_file<char>(file_path);

    return count_tree_hits(map);
}


int64_t sol_3_2(const std::string &file_path)
{
    std::vector<std::vector<char>> map = read_2d_vec_from_file<char>(file_path);
    int64_t result{ 1 };
    using Slope = Point<int>;
    std::vector<Slope> slope_vec = { {1,1}, {3,1}, {5,1}, {7,1}, {1,2} };

    for (auto &slope : slope_vec)
    {
        result *= count_tree_hits(map, slope.x, slope.y);
    }

    return result;
}

int count_tree_hits(const std::vector<std::vector<char>> &map, int step_size_right, int step_size_down)
{
    int num_tree_hits{ 0 };
    int dim_y{ map.size() };
    int dim_x{ map[0].size() };
    Point<int> pos{0,0};

    while (pos.y < dim_y)
    {
        if (TREE == map.at(pos.y).at(pos.x)) ++num_tree_hits;
        pos.x = (pos.x+step_size_right) % dim_x;
        pos.y += step_size_down;
    }

    return num_tree_hits;
}
