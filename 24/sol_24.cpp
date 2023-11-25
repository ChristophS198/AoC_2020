#include <string>
#include <vector>
#include <set>

#include "../utility.h"

using TileCoord = Point<int>; 
using TColor = int; // even corresponds to white and odd to black

std::vector<TileCoord> convert_dir_to_coord(const std::vector<std::string> &dir_vec);
std::set<TileCoord> get_black_tile_set(const std::vector<TileCoord> &tile_coord_vec);
std::set<TileCoord> get_white_tile_set(const std::set<TileCoord> &black_tile_set);
std::pair<std::set<TileCoord>,std::set<TileCoord>> flip_tiles(const std::set<TileCoord> &black_tile_set,const std::set<TileCoord> &white_tile_set);
int get_num_black_neigh_tiles(const TileCoord &tile_pos,const std::set<TileCoord> &black_tile_set);
void add_neighbors_to_white_set(const TileCoord &tile_pos, const std::set<TileCoord> &black_tile_set, std::set<TileCoord> &white_tile_set);
std::ostream& print_set(std::ostream &out, const std::set<TileCoord> &tile_set);

int sol_24_1(const std::string &file_path)
{
    std::vector<std::string> tile_dir_vec = read_string_vec_from_file(file_path);
    std::vector<TileCoord> tile_coord_vec = convert_dir_to_coord(tile_dir_vec);
    std::set<TileCoord> black_tile_set = get_black_tile_set(tile_coord_vec);


    return black_tile_set.size();
}

int sol_24_2(const std::string &file_path)
{
    std::vector<std::string> tile_dir_vec = read_string_vec_from_file(file_path);
    std::vector<TileCoord> tile_coord_vec = convert_dir_to_coord(tile_dir_vec);
    std::set<TileCoord> black_tile_set = get_black_tile_set(tile_coord_vec);
    std::set<TileCoord> white_tile_set = get_white_tile_set(black_tile_set);
    std::pair<std::set<TileCoord>,std::set<TileCoord>> tile_set_pair{black_tile_set,white_tile_set};

    for (int i=0; i<100; ++i)
    {
        tile_set_pair = flip_tiles(tile_set_pair.first, tile_set_pair.second);
    }

    return tile_set_pair.first.size();
}

int get_num_black_neigh_tiles(const TileCoord &tile_pos,const std::set<TileCoord> &black_tile_set)
{
    int cnt_black_neigh_tiles{ 0 };

    // check ne neighbor
    auto neigh = black_tile_set.find(tile_pos + TileCoord{-1,1});
    if (neigh != black_tile_set.end())
    {
        ++cnt_black_neigh_tiles;
    }

    // check e neighbor
    neigh = black_tile_set.find(tile_pos + TileCoord{-2,0});
    if (neigh != black_tile_set.end())
    {
        ++cnt_black_neigh_tiles;
    }

    // check se neighbor
    neigh = black_tile_set.find(tile_pos + TileCoord{-1,-1});
    if (neigh != black_tile_set.end())
    {
        ++cnt_black_neigh_tiles;
    }

    // check nw neighbor
    neigh = black_tile_set.find(tile_pos + TileCoord{1,1});
    if (neigh != black_tile_set.end())
    {
        ++cnt_black_neigh_tiles;
    }

    // check n neighbor
    neigh = black_tile_set.find(tile_pos + TileCoord{2,0});
    if (neigh != black_tile_set.end())
    {
        ++cnt_black_neigh_tiles;
    }

    // check sw neighbor
    neigh = black_tile_set.find(tile_pos + TileCoord{1,-1});
    if (neigh != black_tile_set.end())
    {
        ++cnt_black_neigh_tiles;
    }

    return cnt_black_neigh_tiles;
}


/*
Maybe create two maps: one for black tiles and one for white tiles 
and then always move them from one map to the other + add new tiles if
*/
std::pair<std::set<TileCoord>,std::set<TileCoord>> flip_tiles(const std::set<TileCoord> &black_tile_set,const std::set<TileCoord> &white_tile_set)
{
    std::set<TileCoord> new_black_tiles{ };
    std::set<TileCoord> new_white_tiles{ };
    std::vector<TileCoord> to_be_added;

    // check requirement for white tile
    for (auto &tile : white_tile_set)
    {
        auto num_black_neigh = get_num_black_neigh_tiles(tile, black_tile_set);
        if (num_black_neigh == 2) 
        {
            new_black_tiles.insert(tile);
            to_be_added.push_back(tile);
        }
        else new_white_tiles.insert(tile);
    }

    for (auto &tile : black_tile_set)
    {
        auto num_black_neigh = get_num_black_neigh_tiles(tile, black_tile_set);
        if (num_black_neigh == 0 || num_black_neigh > 2) 
        {
            new_white_tiles.insert(tile);
        }
        else
        {
            new_black_tiles.insert(tile);
            to_be_added.push_back(tile);
        }
    }
    for (auto &tile : to_be_added)
    {
        add_neighbors_to_white_set(tile, new_black_tiles, new_white_tiles);
    }

    return {new_black_tiles,new_white_tiles};
}

std::set<TileCoord> get_white_tile_set(const std::set<TileCoord> &black_tile_set)
{
    std::set<TileCoord> white_tile_set;

    for (const auto &tile_pos : black_tile_set)
    {
        add_neighbors_to_white_set(tile_pos, black_tile_set, white_tile_set);
    }

    return white_tile_set;
}

void add_neighbors_to_white_set(const TileCoord &tile_pos, const std::set<TileCoord> &black_tile_set, std::set<TileCoord> &white_tile_set)
{
            // check ne neighbor
        auto neigh = black_tile_set.find(tile_pos + TileCoord{-1,1});
        if (neigh == black_tile_set.end())
        {
            white_tile_set.insert(tile_pos + TileCoord{-1,1});
        }

        // check e neighbor
        neigh = black_tile_set.find(tile_pos + TileCoord{-2,0});
        if (neigh == black_tile_set.end())
        {
            white_tile_set.insert(tile_pos + TileCoord{-2,0});
        }

        // check se neighbor
        neigh = black_tile_set.find(tile_pos + TileCoord{-1,-1});
        if (neigh == black_tile_set.end())
        {
            white_tile_set.insert(tile_pos + TileCoord{-1,-1});
        }

        // check nw neighbor
        neigh = black_tile_set.find(tile_pos + TileCoord{1,1});
        if (neigh == black_tile_set.end())
        {
            white_tile_set.insert(tile_pos + TileCoord{1,1});
        }

        // check n neighbor
        neigh = black_tile_set.find(tile_pos + TileCoord{2,0});
        if (neigh == black_tile_set.end())
        {
            white_tile_set.insert(tile_pos + TileCoord{2,0});
        }

        // check sw neighbor
        neigh = black_tile_set.find(tile_pos + TileCoord{1,-1});
        if (neigh == black_tile_set.end())
        {
            white_tile_set.insert(tile_pos + TileCoord{1,-1});
        }
}

std::set<TileCoord> get_black_tile_set(const std::vector<TileCoord> &tile_coord_vec)
{
    std::set<TileCoord> black_tile_set;

    for (const auto &tile_coord : tile_coord_vec)
    {
        auto it = black_tile_set.insert(tile_coord);
        if (it.second == false)
        {
            black_tile_set.erase(it.first);
        }
    }

    return black_tile_set;
}

std::ostream& print_set(std::ostream &out, const std::set<TileCoord> &tile_set)
{
    for (const auto &tile : tile_set)
    {
        out << tile.x << "," << tile.y;
        out << "  ";
        // out << "\n";
    }
    out << "\n";
    return out;
}

/*
Possible directions: e, se, sw, w, nw, and ne
Idea is to convert each direction step into a move in either x or y direction
At the end each tile is associated with a 2-d position
*/
std::vector<TileCoord> convert_dir_to_coord(const std::vector<std::string> &dir_vec)
{
    std::vector<TileCoord> coord_vec;
    coord_vec.reserve(dir_vec.size());

    for (const auto &dir_str : dir_vec)
    {
        TileCoord new_coord{0,0};

        for (size_t i=0; i<dir_str.length(); ++i)
        {
            switch (dir_str.at(i))
            {
            case 'e':
                new_coord.x -= 2; // move 2 steps in -x, so ne + se is equivalent to e
                break;
            case 's': // can be either se or sw
                ++i;
                if ('e' == dir_str.at(i)) new_coord.x -= 1;
                else new_coord.x += 1;
                new_coord.y -= 1;
                break;
            case 'w':
                new_coord.x += 2; // move two steps in +x, so nw + sw is equivalent to w
                break;
            case 'n': // can be either ne or nw
                ++i;
                if ('e' == dir_str.at(i)) new_coord.x -= 1;
                else new_coord.x += 1;
                new_coord.y += 1;
                break;
            default:
                throw std::runtime_error("convert_dir_to_coord: invalid dir found: " + std::to_string(dir_str.at(i)));
                break;
            }
        }
        coord_vec.push_back(new_coord);
    }
    return coord_vec;
}