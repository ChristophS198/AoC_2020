#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <algorithm>


#include "../utility.h"

using Allergen = std::string;
using Ingredient = std::string;
using AllergenMap = std::unordered_map<Allergen,std::set<Ingredient>>; // associates one allergen to the ingredients, where it might be contained in
using IngredientMap = std::unordered_map<Ingredient,std::unordered_set<Allergen>>; // associates one ingredient to the allergenes it might contain

struct Food {
    std::vector<Ingredient> ingredients;
    std::vector<Allergen> allergenes;
};

std::vector<Food> get_foods_from_data(const std::string &file_path);
std::vector<Ingredient> get_potential_allergen_free_ingredients();
std::pair<AllergenMap,IngredientMap> get_mapping(const std::vector<Food> &food_vec);
std::map<Allergen,Ingredient> get_ingredient_allergen_map(AllergenMap &allerg_map, IngredientMap &ingred_map);

std::ostream& print_food(std::ostream &out, const Food &food);

int sol_21_1(const std::string &file_path)
{
    std::vector<Food> food_vec = get_foods_from_data(file_path);
    std::pair<AllergenMap,IngredientMap> map_pair = get_mapping(food_vec);
    auto ingred_map = map_pair.second;

    int cnt{ 0 };
    for (const auto &ingred : ingred_map)
    {
        if (ingred.second.empty())
        {
            // iterate through foods and count how often it appears
            for (const auto &food : food_vec)
            {
                if (std::find(food.ingredients.begin(), food.ingredients.end(), ingred.first) != food.ingredients.end())
                {
                    ++cnt;
                }
            }
        }
    }

    return cnt;
}

std::string sol_21_2(const std::string &file_path)
{
    std::vector<Food> food_vec = get_foods_from_data(file_path);
    std::pair<AllergenMap,IngredientMap> map_pair = get_mapping(food_vec);
    auto ingred_map = map_pair.second;
    auto allerg_map = map_pair.first;

    std::map<Allergen,Ingredient> allerg_mapping = get_ingredient_allergen_map(allerg_map, ingred_map);

    // requirement was to sort the ingredient list based on alphabetical order of allergen -> given by using std::map with standard sorting
    std::string canonical_ing_list;
    for (const auto &elem : allerg_mapping)
    {
        canonical_ing_list += elem.second + ",";
    }
    canonical_ing_list.pop_back();
    return canonical_ing_list;
}

std::map<Allergen,Ingredient> get_ingredient_allergen_map(AllergenMap &allerg_map, IngredientMap &ingred_map)
{
    // Repeatedly iterate through allerg_map and search for 1-to-1 mappings
    bool still_changing{ true };
    std::map<Allergen,Ingredient> allergen_mapping;
    while(still_changing)
    {
        still_changing = false;
        std::vector<Allergen> newly_matched_allergens;

        for (auto &allerg : allerg_map)
        {
            if (allerg.second.size() == 1)
            {
                // 1-to-1 mapping found -> this ingredient can be removed from all other allergens candidates lists
                auto associated_ing = *allerg.second.begin();
                allergen_mapping[allerg.first] = associated_ing;
                newly_matched_allergens.push_back(allerg.first);
                
                // remove this ingredient from all allergene candidate lists
                for (auto &allerg_cand : ingred_map[associated_ing])
                {
                    still_changing = true;
                    allerg_map[allerg_cand].erase(associated_ing);
                    // ingred.second.erase(allerg.first);
                }
            }
        }
        for (auto &elem : newly_matched_allergens)
        {
            allerg_map.erase(elem);
        }

    }

    return allergen_mapping;
}

std::pair<AllergenMap,IngredientMap> get_mapping(const std::vector<Food> &food_vec)
{
    AllergenMap allergen_map;
    IngredientMap ingred_map;

    for (const auto &food : food_vec)
    {
        for (const auto &ingred : food.ingredients)
        {
            ingred_map[ingred].insert(food.allergenes.begin(), food.allergenes.end());
        }
        for (const auto &allerg : food.allergenes)
        {
            auto allerg_cur = allergen_map.find(allerg);
            if (allerg_cur == allergen_map.end())
            {
                // if allergen does not exist in map -> insert complete ingredient vector
                allergen_map[allerg].insert(food.ingredients.begin(), food.ingredients.end());
            }
            else
            {
                std::set<Ingredient> ingredient_intersection;
                auto &ingred_candidates = allerg_cur->second;
                // two steps are done here:
                // 1. Intersecting elements: 
                    // create intersection of this foods ingredients with already existing ingredient candidates of this allergen (ingred_candidates)
                // 2. Non-intersecting elements:
                    // find ingredients that are no longer a candidate for this allergene and remove the allergen in the corresponding ingred_map entries
                for (auto &i : food.ingredients)
                {
                    if (ingred_candidates.count(i) > 0)
                    {
                        // ingredient stays a candidate -> add to intersection and remove from old candidates set
                        ingredient_intersection.insert(i);
                        ingred_candidates.erase(i);
                    }
                    else
                    {
                        // this allergene cannot be part of ingredient i -> remove it from ingred_map[i]
                        ingred_map[i].erase(allerg);
                    }
                }
                for (auto &i : ingred_candidates) // remove the non-intersecting elements of ingred_candidates from ingred_map
                {
                    // this allergene cannot be part of ingredient i -> remove it from ingred_map[i]
                    ingred_map[i].erase(allerg);
                }
                ingred_candidates = ingredient_intersection;
                if (ingredient_intersection.empty()) throw std::runtime_error("Error: Allergene " + allerg + " has no ingredient candidates left!");

            }
        }
    }

    return { allergen_map,ingred_map };
}



std::vector<Food> get_foods_from_data(const std::string &file_path)
{
    std::vector<Food> food_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        // read foods from file
        while(getline(input_file, input_line)){
            Food nxt_food{};
            std::vector<std::string> split_res = split_string(input_line,"(contains ");

            // insert ingredients
            std::string ing;
            for (auto c : split_res[0])
            {
                if (c==' ')
                {
                    nxt_food.ingredients.push_back(ing);
                    ing.clear();
                }
                else
                {
                    ing += c;
                }
            }

            // append allergenes
            std::string allerg;
            for (auto c : split_res[1])
            {
                if (c==',' || c==')')
                {
                    nxt_food.allergenes.push_back(allerg);
                    allerg.clear();
                }
                else
                {
                    if (c==' ') continue;
                    else allerg += c;
                }
            }
            food_vec.push_back(nxt_food);
            // print_food(std::cout, nxt_food);
        }
        input_file.close();   
    }
    return food_vec;
}

std::ostream& print_food(std::ostream &out, const Food &food)
{
    for (const auto &ing : food.ingredients)
    {
        out << ing << " ";
    }
    out << "(contains ";
    for (const auto &allerg : food.allergenes)
    {
        out << allerg << ", ";
    }
    out << ")\n";
    return out;
}