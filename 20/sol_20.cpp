#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

#include "../utility.h"

using Tile = std::vector<std::string>;
using BorderHash = uint16_t;
using TileId = int;
using BorderIdStr = std::string;

constexpr char COL{ '#' }; // stands for color in monochrome tile
constexpr char NO_COL{ '.' }; // stands for no color 

const std::unordered_map<BorderIdStr,int> hash_idx_map{
    {"Left",0}, {"LeftInv",1},{"Top",2}, {"TopInv",3},
    {"Right",4}, {"RightInv",5},{"Bottom",6}, {"BottomInv",7},
};

struct TileInfo {
    std::vector<BorderHash> border_hashes;
    Tile inner_tile;
    TileId id;
};

struct SeaMonster {
    std::vector<Point<size_t>> coords; // sea monster is deifned by the locations of '#'
    Point<size_t> monster_dim;
} ;

using tile_info_map = std::unordered_map<TileId, TileInfo>;
using border_hash_map = std::unordered_map<BorderHash, std::vector<std::pair<TileId,BorderIdStr>>>; // fast lookup of Tiles with same border hash

std::pair<tile_info_map, border_hash_map> read_tiles_from_file(const std::string &file_path);
std::vector<BorderHash> calc_hashes(const std::vector<std::string> &borders);
std::vector<TileId> get_four_corner_tiles(const border_hash_map &hash_map);
std::ostream& print_tile(std::ostream &out, const Tile &tile);
std::ostream& print_tile_id_map(std::ostream &out, const std::vector<std::vector<TileId>> &tile_placement);


// part 2
void rotate_tile_info_clockwise(TileInfo &tile_in);
void flip_tile_info_left_right(TileInfo &tile_in);
void flip_tile_info_up_down(TileInfo &tile_in);
void align_tile_right(const TileInfo &tile_l, TileInfo &tile_r, const BorderIdStr &id_str);
void align_tile_below(const TileInfo &tile_l, TileInfo &tile_r, const BorderIdStr &id_str);
Tile rotate_tile_clockwise(const Tile &tile_in);
Tile flip_tile_up_down(const Tile &tile_in);
Tile flip_tile_left_right(const Tile &tile_in);
Point<size_t> insert_tile_right_below(const TileInfo &tile, std::vector<std::string> &camera_image, Point<size_t> insert_pos);
void align_top_left_corner(TileInfo &top_left_tile, const border_hash_map &hash_map);
SeaMonster rotate_monster_clockwise(const SeaMonster &monster);
std::vector<SeaMonster> get_sea_monster();
int get_water_roughness(const std::vector<std::string> &cam_image, const std::vector<SeaMonster> &monsters);
bool is_monster(size_t row, size_t col, const std::vector<std::string> &cam_image, const SeaMonster &monster);



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
    auto &info_map = tile_data.first;
    auto &hash_map = tile_data.second;
    std::vector<TileId> corner_t = get_four_corner_tiles(hash_map);
    auto &left_top_corner = info_map[corner_t[2]];

    // reserve space for reassembled camera image
    size_t dim = std::sqrt(info_map.size());
    size_t dim_single_tile = left_top_corner.inner_tile.size();
    std::vector<std::string> camera_image(dim*dim_single_tile, std::string(dim*dim_single_tile,NO_COL));
    std::vector<std::vector<TileId>> tile_placement;

    // 1. take one of the corner tiles and rotate it so the unique borders are the left and top ones
    flip_tile_info_left_right(left_top_corner); // flip corner so it matches the example of day 20_2 (only for convenience)
    align_top_left_corner(left_top_corner, hash_map);
    Point<size_t> insert_pos{ 0,0 };
    insert_pos = insert_tile_right_below(left_top_corner, camera_image, insert_pos);


    // 2. take the right and bootom border of left, top-most corner tile and search for matching tiles to the right until a unique border is hit (== square)
    // the new tiles must be rotated and/or flipped and inserted in the large 2d camera image
    // after hitting a unique right border, take the bottom neighbor of left, top-most corner tile and again search for matching tiles to the right until a unique border is found
    auto cur_tile_info = left_top_corner;
    auto last_start_tile_info = cur_tile_info;
    auto right_border_hash = cur_tile_info.border_hashes[hash_idx_map.at("Right")];
    auto potential_neigh_right = hash_map[right_border_hash];
    auto bottom_border_hash = cur_tile_info.border_hashes[hash_idx_map.at("Bottom")];
    auto potential_neigh_below = hash_map[bottom_border_hash];
    tile_placement.push_back( {cur_tile_info.id} );

    while (insert_pos.x < camera_image.size())
    {

        while (potential_neigh_right.size() > 1) 
        {
            // as long as potential_neigh_right contains two entires for a hash there exists a right neighbor
            auto neigh_right = potential_neigh_right[0].first == cur_tile_info.id ? potential_neigh_right[1] : potential_neigh_right[0];
            auto neigh_right_tile_info = info_map[neigh_right.first];
            align_tile_right(cur_tile_info, neigh_right_tile_info, neigh_right.second);
            insert_pos = insert_tile_right_below(neigh_right_tile_info, camera_image, insert_pos);
            tile_placement.back().push_back(neigh_right_tile_info.id);
            cur_tile_info = neigh_right_tile_info;
            right_border_hash = cur_tile_info.border_hashes[hash_idx_map.at("Right")];
            potential_neigh_right = hash_map[right_border_hash];
        }
        // Find first tile of next row -> must match bootom border of first tile in current row
        insert_pos.x += dim_single_tile;
        if (insert_pos.x >= camera_image.size()) break;
        insert_pos.y = 0;
        // insert first tile of new rows
        auto neigh_below = potential_neigh_below[0].first == last_start_tile_info.id ? potential_neigh_below[1] : potential_neigh_below[0];
        auto neigh_below_tile_info = info_map[neigh_below.first];
        align_tile_below(last_start_tile_info, neigh_below_tile_info, neigh_below.second);
        insert_pos = insert_tile_right_below(neigh_below_tile_info, camera_image, insert_pos);
        tile_placement.push_back({ neigh_below_tile_info.id });

        // update values for next loop
        last_start_tile_info = neigh_below_tile_info;
        cur_tile_info = neigh_below_tile_info;
        bottom_border_hash = cur_tile_info.border_hashes[hash_idx_map.at("Bottom")];
        potential_neigh_below = hash_map[bottom_border_hash];
        right_border_hash = cur_tile_info.border_hashes[hash_idx_map.at("Right")];
        potential_neigh_right = hash_map[right_border_hash];

    }
    // std::ofstream out("tile_id_map.txt");
    // print_tile_id_map(out, tile_placement);
    // out.close();
    // std::ofstream out2("camera_image.txt");
    // print_tile(out2, camera_image);
    // out2.close();

    // instead of rotating the camera image, create a vector of rotated/flipped seamonsters and move them over the cam image 
    auto monsters = get_sea_monster();
    return get_water_roughness(camera_image, monsters);


}


int get_water_roughness(const std::vector<std::string> &cam_image, const std::vector<SeaMonster> &monsters)
{
    int num_monsters{ 0 };
    int num_color_elem{ 0 };
    size_t rows{ cam_image.size() };
    size_t cols{ cam_image.at(0).size() };
    for (size_t row=0; row<rows; ++row)
    {
        for (size_t col=0; col<cols; ++col)
        {
            for (const auto &monster : monsters)
            {
                if (is_monster(row,col,cam_image,monster)) 
                {
                    ++num_monsters;
                }
            }
            if (cam_image[row][col] == COL) ++num_color_elem;
        }
    }

    return num_color_elem - num_monsters*monsters.at(0).coords.size();
}

bool is_monster(size_t row, size_t col, const std::vector<std::string> &cam_image, const SeaMonster &monster)
{
    auto rows{ cam_image.size() };
    auto cols{ cam_image.at(0).size() };
    for (auto coord : monster.coords)
    {
        
        if (row+coord.x >= rows || col+coord.y >= cols ||
            cam_image[row+coord.x][col+coord.y] != COL) 
            {
                return false;
            }
    }

    return true;
}

/*
Sea monster:
                  # 
#    ##    ##    ###
 #  #  #  #  #  #   
*/
std::vector<SeaMonster> get_sea_monster()
{
    std::vector<SeaMonster> sea_monster_vec;
    Point<size_t> monster_dim{3,20};
    SeaMonster sea_monster;
    sea_monster.coords = { {0,18},
        {1,0},{1,5},{1,6},{1,11},{1,12},{1,17},{1,18},{1,19}, 
        {2,1},{2,4},{2,7},{2,10},{2,13},{2,16} };
    sea_monster.monster_dim = {3,20};
    sea_monster_vec.push_back(sea_monster);

    // flip sea monster left right
    SeaMonster flipped_l_r_monster;

    for (auto point : sea_monster.coords)
    {
        flipped_l_r_monster.coords.push_back({point.x, monster_dim.y-1-point.y});
    }
    flipped_l_r_monster.monster_dim = {sea_monster.monster_dim.y,sea_monster.monster_dim.x};
    sea_monster_vec.push_back(flipped_l_r_monster);

    // rotate monser and flipped monster 3 times clockwise in order to get each pattern
    for (int i=0; i<3; i++)
    {
        sea_monster_vec.push_back(rotate_monster_clockwise(sea_monster_vec[i*2]));
        sea_monster_vec.push_back(rotate_monster_clockwise(sea_monster_vec[i*2+1]));
    }

    return sea_monster_vec;
}

SeaMonster rotate_monster_clockwise(const SeaMonster &monster)
{
    SeaMonster rotated_monster;

    for (auto point : monster.coords)
    {
        rotated_monster.coords.push_back({point.y, monster.monster_dim.x-1-point.x});
    }
    rotated_monster.monster_dim = {monster.monster_dim.y,monster.monster_dim.x};
    return rotated_monster;
}

Point<size_t> insert_tile_right_below(const TileInfo &tile, std::vector<std::string> &camera_image, Point<size_t> insert_pos)
{
    size_t tile_dim{ tile.inner_tile.size() };
    auto ins_x = camera_image[insert_pos.x].begin()+insert_pos.y;

    for (int row=0; row<tile_dim; ++row)
    {
        auto &cur_row = camera_image[row+insert_pos.x];
        for (int col=0; col<tile_dim; ++col)
        {   
            cur_row[col+insert_pos.y] = tile.inner_tile[row][col];
        }
    }
    insert_pos.y += tile_dim;
    return insert_pos;
}

void align_tile_right(const TileInfo &tile_l, TileInfo &tile_r, const BorderIdStr &id_str)
{
    switch (hash_idx_map.at(id_str))
    {
    case 0: // left border matches
        // do nothing
        break;
    case 1: // left inverse border matches
        flip_tile_info_up_down(tile_r);
        break;    
    case 2: // top border matches
        rotate_tile_info_clockwise(tile_r);
        flip_tile_info_left_right(tile_r);
        break;    
    case 3: // top inverse border matches
        rotate_tile_info_clockwise(tile_r);
        flip_tile_info_left_right(tile_r);
        flip_tile_info_up_down(tile_r);
        break;
    case 4: // right border matches -> must be moved to the left
        flip_tile_info_left_right(tile_r);
        break;
    case 5: // right inverse border matches -> must be moved to the left
        flip_tile_info_left_right(tile_r);
        flip_tile_info_up_down(tile_r);
        break;
    case 6: // bottom border matches -> must be moved to the left
        rotate_tile_info_clockwise(tile_r);
        break;
    case 7: // bottom inverse border matches -> must be moved to the left
        rotate_tile_info_clockwise(tile_r);
        flip_tile_info_up_down(tile_r);
        break;
    default:
        break;
    }
}

void align_tile_below(const TileInfo &tile_l, TileInfo &tile_r, const BorderIdStr &id_str)
{
    switch (hash_idx_map.at(id_str))
    {
    case 0: // left border matches
        rotate_tile_info_clockwise(tile_r);
        flip_tile_info_left_right(tile_r);
        break;
    case 1: // left inverse border matches
        rotate_tile_info_clockwise(tile_r);
        break;    
    case 2: // top border matches
        // do nothing
        break;    
    case 3: // top inverse border matches
        flip_tile_info_left_right(tile_r);
        break;
    case 4: // right border matches -> must be moved to the top
        rotate_tile_info_clockwise(tile_r);
        rotate_tile_info_clockwise(tile_r);
        rotate_tile_info_clockwise(tile_r);
        break;
    case 5: // right inverse border matches -> must be moved to the top
        rotate_tile_info_clockwise(tile_r);
        flip_tile_info_up_down(tile_r);
        break;
    case 6: // bottom border matches -> must be moved to the top
        flip_tile_info_up_down(tile_r);
        break;
    case 7: // bottom inverse border matches -> must be moved to the top
        flip_tile_info_left_right(tile_r);
        flip_tile_info_up_down(tile_r);
        break;
    default:
        break;
    }
}

std::vector<TileId> get_four_corner_tiles(const border_hash_map &hash_map)
{
    std::vector<TileId> corner_tiles;
    std::unordered_map<TileId,int> margin_border_map; // map that stores how many borders of a tile are unique -> if 2*2 (*2 due to reverse and forward order of border) are unique it must be a corner
    for (const auto &hash : hash_map)
    {
        if (hash.second.size() == 1) 
        {
            ++margin_border_map[hash.second[0].first];
            if (margin_border_map[hash.second[0].first] == 4) corner_tiles.push_back(hash.second[0].first);
        }
    }
    return corner_tiles;
}

void align_top_left_corner(TileInfo &top_left_tile, const border_hash_map &hash_map)
{
    int idx_l = hash_idx_map.at("Left");
    int idx_t = hash_idx_map.at("Top");
    auto hash_left = top_left_tile.border_hashes[idx_l];
    auto hash_top = top_left_tile.border_hashes[idx_t];
    while (hash_map.at(hash_left).size() != 1 or hash_map.at(hash_top).size() != 1)
    {
        rotate_tile_info_clockwise(top_left_tile);
        hash_left = top_left_tile.border_hashes[idx_l];
        hash_top = top_left_tile.border_hashes[idx_t];
    }
}

void rotate_tile_info_clockwise(TileInfo &tile_in)
{
    tile_in.inner_tile = rotate_tile_clockwise(tile_in.inner_tile);
    // next also adapt border hashes to new tile
    auto tmp = tile_in.border_hashes;
    tile_in.border_hashes[0] = tmp[6];
    tile_in.border_hashes[1] = tmp[7];
    tile_in.border_hashes[2] = tmp[1];
    tile_in.border_hashes[3] = tmp[0];
    tile_in.border_hashes[4] = tmp[2];
    tile_in.border_hashes[5] = tmp[3];
    tile_in.border_hashes[6] = tmp[5];
    tile_in.border_hashes[7] = tmp[4];
}

void flip_tile_info_up_down(TileInfo &tile_in)
{
    tile_in.inner_tile = flip_tile_up_down(tile_in.inner_tile);
    // next also adapt border hashes to new tile
    auto tmp = tile_in.border_hashes;
    tile_in.border_hashes[0] = tmp[1];
    tile_in.border_hashes[1] = tmp[0];
    tile_in.border_hashes[2] = tmp[6];
    tile_in.border_hashes[3] = tmp[7];
    tile_in.border_hashes[4] = tmp[5];
    tile_in.border_hashes[5] = tmp[4];
    tile_in.border_hashes[6] = tmp[2];
    tile_in.border_hashes[7] = tmp[3];
}

void flip_tile_info_left_right(TileInfo &tile_in)
{
    tile_in.inner_tile = flip_tile_left_right(tile_in.inner_tile);
    // next also adapt border hashes to new tile
    auto tmp = tile_in.border_hashes;
    tile_in.border_hashes[0] = tmp[4];
    tile_in.border_hashes[1] = tmp[5];
    tile_in.border_hashes[2] = tmp[3];
    tile_in.border_hashes[3] = tmp[2];
    tile_in.border_hashes[4] = tmp[0];
    tile_in.border_hashes[5] = tmp[1];
    tile_in.border_hashes[6] = tmp[7];
    tile_in.border_hashes[7] = tmp[6];
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

Tile flip_tile_up_down(const Tile &tile_in)
{
    size_t w{ tile_in.at(0).size() };
    size_t h{ tile_in.size() };

    Tile tile_flipped(h,  std::string(w,NO_COL));
    for (size_t row=0; row<h; ++row)
    {
        tile_flipped[h-row-1] = tile_in[row];
    }

    return tile_flipped;
}

Tile flip_tile_left_right(const Tile &tile_in)
{
    size_t w{ tile_in.at(0).size() };
    size_t h{ tile_in.size() };

    Tile tile_flipped(h,  std::string(w,NO_COL));
    for (size_t row=0; row<h; ++row)
    {
        for (size_t col=0; col<w; ++col)
        {
            tile_flipped[row][w-col-1] = tile_in[row][col];
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
        Tile inner_tile;

        // read tiles from file
        while(getline(input_file, input_line)){
            if ("" == input_line)
            {
                // process old tile by inserting respective elements in both maps
                TileInfo tile_info{};
                tile_info.border_hashes = calc_hashes({left_b, top_b, right_b, bottom_b});
                tile_info.id = tile_id;
                inner_tile.pop_back();
                tile_info.inner_tile = inner_tile;
                t_info_map[tile_id] = tile_info;
                for (const auto &hash : hash_idx_map)
                {
                    hash_map[tile_info.border_hashes[hash.second]].push_back({ tile_id, hash.first });
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
                inner_tile.clear();

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
                inner_tile.push_back({input_line.begin()+1, input_line.end()-1});
            }
        }
        TileInfo tile_info{};
        tile_info.border_hashes = calc_hashes({left_b, top_b, right_b, bottom_b});
        tile_info.id = tile_id;
        inner_tile.pop_back();
        tile_info.inner_tile = inner_tile;
        t_info_map[tile_id] = tile_info;
        for (const auto &hash : hash_idx_map)
        {
            hash_map[tile_info.border_hashes[hash.second]].push_back({ tile_id, hash.first });
        }
        input_file.close();   
    }
    return { t_info_map,hash_map };
}

/*
calcs a special id for each border string 
adds two hashes for each border: one for normal hash and one for reverse border
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

        hashes.push_back(reverse_hash);
        hashes.push_back(forward_hash);
    }
    return hashes;
}

std::ostream& print_tile_id_map(std::ostream &out, const std::vector<std::vector<TileId>> &tile_placement)
{
    out << "\n";

    for (const auto &row : tile_placement)
    {
        for (const auto &elem : row)
        {
            out << std::setw(4) << elem << " ";
        }
        out << "\n";
    }
    out << "\n";

    return out;
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