#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "../utility.h"

using Count = uint32_t;
using Grid2D = std::vector<std::vector<char>>;
using Grid3D = std::vector<Grid2D>;
using Grid4D = std::vector<Grid3D>;

constexpr char ACTIVE_CUBE = '#';
constexpr char INACTIVE_CUBE = '.';

std::pair<Count,Grid3D> do_cycle(const Grid3D &grid);
std::pair<Count,Grid4D> do_cycle(const Grid4D &start_grid);
std::vector<std::vector<char>> read_grid_from_file(const std::string& file_path);
int get_active_neighbors_4d(const Grid4D &grid, int px, int py, int pz, int pw);


int sol_17_1(const std::string &file_path)
{
    Grid2D grid_2d = read_grid_from_file(file_path);
    Grid3D grid_3d(5, std::vector<std::vector<char>>(grid_2d.size(), std::vector<char>(grid_2d[0].size(), INACTIVE_CUBE)));
    grid_3d[2] = grid_2d;
    Count act_cnt{ 0 };

    for (int i=0; i<6; ++i)
    {
        auto res = do_cycle(grid_3d);
        grid_3d = res.second;
        act_cnt = res.first;
    }

    return act_cnt;
}


int64_t sol_17_2(const std::string &file_path)
{
    Grid2D grid_2d = read_grid_from_file(file_path);
    Grid4D grid_4d(5, Grid3D(5, Grid2D(grid_2d.size(), std::vector<char>(grid_2d[0].size(), INACTIVE_CUBE))));
    grid_4d[2][2] = grid_2d;
    Count act_cnt{ 0 };

    for (int i=0; i<6; ++i)
    {
        auto res = do_cycle(grid_4d);
        grid_4d = res.second;
        act_cnt = res.first;
    }

    return act_cnt;
}


int get_active_neighbors_3d(const Grid3D &grid, int px, int py, int pz)
{
    int act_cnt{ 0 };
    for (int z=-1; z<2; ++z)
    {
        auto z_new{ pz - z};
        for (int y=-1; y<2; ++y)
        {
            auto y_new = py-y;
            for (int x=-1; x<2; ++x)
            {
                auto x_new{ px - x};
                if (grid.at(z_new).at(y_new).at(x_new) == ACTIVE_CUBE)
                {
                    ++act_cnt;
                }
            }
        }
    }
    if (grid.at(pz).at(py).at(px) == ACTIVE_CUBE)
    {
        --act_cnt;
    }
    return act_cnt < 0 ? 0 : act_cnt;
}

int get_active_neighbors_4d(const Grid4D &grid, int px, int py, int pz, int pw)
{
    int act_cnt{ 0 };
    for (int w=-1; w<2; ++w)
    {
        auto w_new{ pw-w };
        for (int z=-1; z<2; ++z)
        {
            auto z_new{ pz - z};
            for (int y=-1; y<2; ++y)
            {
                auto y_new = py-y;
                for (int x=-1; x<2; ++x)
                {
                    auto x_new{ px - x};
                    if (grid.at(w_new).at(z_new).at(y_new).at(x_new) == ACTIVE_CUBE)
                    {
                        ++act_cnt;
                    }
                }
            }
        }
    }
    if (grid.at(pw).at(pz).at(py).at(px) == ACTIVE_CUBE)
    {
        --act_cnt;
    }
    return act_cnt < 0 ? 0 : act_cnt;
}

std::pair<Count,Grid4D> do_cycle(const Grid4D &start_grid)
{
    // create new grid with increased dimension (+1 in each direction) and init with inactive cubes
    int w_size{ start_grid.size() };
    int z_size{ start_grid.at(0).size() };
    int y_size{ start_grid.at(0).at(0).size() };
    int x_size{ start_grid.at(0).at(0).at(0).size() };
    Grid4D new_grid(w_size+2, Grid3D(z_size+2, Grid2D(y_size+2, std::vector<char>(x_size+2, INACTIVE_CUBE))));

    Count act_cnt{ 0 };

    // loop through inner points -> no border checks
    for (int w=1; w<w_size-1; ++w)
    {
        for (int z=1; z<z_size-1; ++z)
        {
            for (int y=1; y<y_size-1; ++y)
            {
                for (int x=1; x<x_size-1; ++x)
                {
                    auto act_neigh_cnt = get_active_neighbors_4d(start_grid, x,y,z,w);
                    // z: 2, y=2, x=3
                    if (ACTIVE_CUBE == start_grid.at(w).at(z).at(y).at(x))
                    {
                        if (act_neigh_cnt==2 || act_neigh_cnt==3)
                        {
                            new_grid[w+1][z+1][y+1][x+1] = ACTIVE_CUBE;
                            ++act_cnt;
                        }
                    }
                    else
                    {
                        if (act_neigh_cnt==3)
                        {
                            new_grid[w+1][z+1][y+1][x+1] = ACTIVE_CUBE;
                            ++act_cnt;
                        }
                    }
                }
            }
        }
    }

    return std::pair<Count,Grid4D>{act_cnt,new_grid};
}

std::pair<Count,Grid3D> do_cycle(const Grid3D &start_grid)
{
    // create new grid with increased dimension (+1 in each direction) and init with inactive cubes
    int z_size{ start_grid.size() };
    int y_size{ start_grid.at(0).size() };
    int x_size{ start_grid.at(0).at(0).size() };
    Grid3D new_grid(z_size+2, std::vector<std::vector<char>>(y_size+2, std::vector<char>(x_size+2, INACTIVE_CUBE)));

    Count act_cnt{ 0 };

    // loop through inner points -> no border checks
    for (int z=1; z<z_size-1; ++z)
    {
        for (int y=1; y<y_size-1; ++y)
        {
            for (int x=1; x<x_size-1; ++x)
            {
                auto act_neigh_cnt = get_active_neighbors_3d(start_grid, x,y,z);
                // z: 2, y=2, x=3
                if (ACTIVE_CUBE == start_grid.at(z).at(y).at(x))
                {
                    if (act_neigh_cnt==2 || act_neigh_cnt==3)
                    {
                        new_grid[z+1][y+1][x+1] = ACTIVE_CUBE;
                        ++act_cnt;
                    }
                }
                else
                {
                    if (act_neigh_cnt==3)
                    {
                        new_grid[z+1][y+1][x+1] = ACTIVE_CUBE;
                        ++act_cnt;
                    }
                }
            }
        }
    }

    return std::pair<Count,Grid3D>{act_cnt,new_grid};
}


std::vector<std::vector<char>> read_grid_from_file(const std::string& file_path)
{
    std::vector<std::vector<char>> char_vec_2d{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            if (char_vec_2d.empty())
            {
                // insert two inactive rows at the beginning
                char_vec_2d.push_back(std::vector<char>(input_line.length()+4, INACTIVE_CUBE));
                char_vec_2d.push_back(std::vector<char>(input_line.length()+4, INACTIVE_CUBE));
            }
            char_vec_2d.push_back({});
            char_vec_2d.back().reserve(char_vec_2d[0].size());
            char_vec_2d.back().push_back(INACTIVE_CUBE);
            char_vec_2d.back().push_back(INACTIVE_CUBE);
            char_vec_2d.back().insert(char_vec_2d.back().end(), input_line.begin(), input_line.end());
            char_vec_2d.back().push_back(INACTIVE_CUBE);
            char_vec_2d.back().push_back(INACTIVE_CUBE);
        }
        input_file.close();   

        // insert two inactive rows at the end
        char_vec_2d.push_back(std::vector<char>(char_vec_2d[0].size(), INACTIVE_CUBE));
        char_vec_2d.push_back(std::vector<char>(char_vec_2d[0].size(), INACTIVE_CUBE));
    }
    return char_vec_2d;
}