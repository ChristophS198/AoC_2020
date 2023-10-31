#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <memory>

#include "../utility.h"

using Tile = std::vector<std::string>;
using BorderHash = uint16_t;
using TileId = int;

constexpr char COL{ '#' }; // stands for color in monochrome tile
constexpr char NO_COL{ '.' }; // stands for no color 

enum class Orientation{
    Up, // No rotation from original tile
    Right, // 90 degree clockwise rotation from original tile
    Down, // 180 degree
    Left, // 270 degree  
    FlipLeftRight, // mirrored vertically 
    FlipUpDown // mirrored horizontally
};

struct TileInfo {
    std::vector<BorderHash> border_hashes;
    Point<int> pos;
    Orientation orient;
    TileId id;
};

using tile_info_map = std::unordered_map<TileId, TileInfo>;
using border_hash_map = std::unordered_map<BorderHash, std::vector<TileId>>; // fast lookup of Tiles with same border hash

Tile rotate_tile_clockwise(const Tile &tile_in);
Tile flip_tile(const Tile &tile_in);

std::pair<tile_info_map, border_hash_map> read_tiles_from_file(const std::string &file_path);
std::vector<BorderHash> calc_hashes(const std::vector<std::string> &borders);
std::vector<TileId> get_four_corner_tiles(const border_hash_map &hash_map);
std::ostream& print_tile(std::ostream &out, const Tile &tile);

// part 2
void align_tile_right(const TileInfo &tile_l, TileInfo &tile_r, const BorderHash common_hash);
void align_tile_bottom(const TileInfo &tile_l, TileInfo &tile_r, const BorderHash common_hash);

uint64_t sol_20_1(const std::string &file_path)
{
    std::pair<tile_info_map, border_hash_map> tile_data = read_tiles_from_file(file_path);
    auto &hash_map = tile_data.second;
    std::vector<TileId> corner_t = get_four_corner_tiles(hash_map);

    return (uint64_t)corner_t[0] * corner_t[1] * corner_t[2] * corner_t[3];
}


int sol_20_2(const std::string &file_path)
{
    std::pair<tile_info_map, border_hash_map> tile_data = read_tiles_from_file(file_path);
    auto &hash_map = tile_data.second;
    std::vector<TileId> corner_t = get_four_corner_tiles(hash_map);

    // 1. take one of the corner tiles and rotate it so the unique borders are the left and top ones

    // 2. take the right border of left, top-most corner tile and search for matching tiles to the right until a unique border is hit (== square)
    // here the new tiles must be rotated and/or flipped and inserted in the large 2d array

    // 3. find a matching bottom neighbor of left, top-most corner tile and repeat step 2

    return 0;
}


void align_tile_bottom(const TileInfo &tile_l, TileInfo &tile_r, const BorderHash common_hash)
{
    int s;
}

std::vector<TileId> get_four_corner_tiles(const border_hash_map &hash_map)
{
    std::vector<TileId> corner_tiles;
    std::unordered_map<TileId,int> margin_border_map; // map that stores how many borders of a tile a unique -> if 2 are unique it must be a corner
    for (const auto &hash : hash_map)
    {
        if (hash.second.size() == 1) 
        {
            ++margin_border_map[hash.second[0]];
            if (margin_border_map[hash.second[0]] == 2) corner_tiles.push_back(hash.second[0]);
        }
    }
    return corner_tiles;
}

/*
single clockwise rotation by 90 degree
*/
Tile rotate_tile_clockwise(const Tile &tile_in)
{
    size_t new_w{ tile_in.size() };
    size_t new_h{ tile_in.at(0).size() };

    Tile tile_rotated(new_h,  std::string(new_w,NO_COL));

    for (size_t row=0; row<new_w; ++row)
    {
        for (size_t col=0; col<new_h; ++col)
        {
            tile_rotated[col][new_h-row-1] = tile_in[row][col];
        }
    }

    return tile_rotated;
}

Tile flip_tile(const Tile &tile_in)
{
    size_t w{ tile_in.at(0).size() };
    size_t h{ tile_in.size() };

    Tile tile_flipped(h,  std::string(w,NO_COL));
    for (size_t row=0; row<h; ++row)
    {
        for (size_t col=0; col<w; ++col)
        {
            tile_flipped[h-row-1][col] = tile_in[row][col];
        }
    }

    return tile_flipped;
}

std::pair<tile_info_map, border_hash_map> read_tiles_from_file(const std::string &file_path)
{
    tile_info_map t_info_map;
    border_hash_map hash_map;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;

        getline(input_file, input_line); // read first tile id
        int tile_id = get_number_after_str<int>(input_line, "Tile ");
        getline(input_file, input_line); // read first line of first tile
        std::string top_b = input_line;
        std::string left_b{ top_b.front() };
        std::string right_b{ top_b.back() };
        std::string bottom_b{};

        // read tiles from file
        while(getline(input_file, input_line)){
            if ("" == input_line)
            {
                // process old tile by inserting respective elements in both maps
                TileInfo tile{};
                tile.border_hashes = calc_hashes({left_b, top_b, right_b, bottom_b});
                tile.id = tile_id;
                t_info_map[tile_id] = tile;
                for (const auto &hash : tile.border_hashes)
                {
                    hash_map[hash].push_back(tile_id);
                }
                {
                    int tmp{ 0 };
                    for (auto elem : hash_map)
                    {
                        tmp += elem.second.size();
                    }
                }
                left_b.clear();
                right_b.clear();

                // start processing of next tile
                getline(input_file, input_line);
                tile_id = get_number_after_str<int>(input_line, "Tile ");
                getline(input_file, input_line);
                top_b = input_line;
                left_b += input_line[0];
                right_b += input_line.back();
            }
            else
            {
                left_b += input_line[0];
                right_b += input_line.back();
                bottom_b = input_line; // use bottom_b as buffer for last line
            }
        }
        TileInfo tile{};
        tile.border_hashes = calc_hashes({left_b, top_b, right_b, bottom_b});
        tile.id = tile_id;
        t_info_map[tile_id] = tile;
        for (const auto &hash : tile.border_hashes)
        {
            hash_map[hash].push_back(tile_id);
        }
        input_file.close();   
    }
    return { t_info_map,hash_map };
}

/*
calcs a special id for each border string 
id is special in the sense that it is the same irrespective of forward or reverse order 
*/
std::vector<BorderHash> calc_hashes(const std::vector<std::string> &borders)
{
    std::vector<BorderHash> hashes;
    auto tile_dim = borders.at(0).length();
    for (const auto &border : borders)
    {
        BorderHash forward_hash{ 0 };
        BorderHash reverse_hash{ 0 };

        for (size_t i=0; i<tile_dim; ++i)
        {
            forward_hash <<= 1;
            reverse_hash <<= 1;
            if (COL == border[i]) forward_hash += 1;
            if (COL == border[tile_dim-i-1]) reverse_hash += 1;
        }

        if (reverse_hash < forward_hash)
        {
            hashes.push_back(reverse_hash);
        }
        else
        {
            hashes.push_back(forward_hash);
        }
    }
    return hashes;
}

std::ostream& print_tile(std::ostream &out, const Tile &tile)
{
    out << "\n";

    for (const auto &row : tile)
    {
        for (const auto &elem : row)
        {
            out << elem;
        }
        out << "\n";
    }
    out << "\n";

    return out;
}