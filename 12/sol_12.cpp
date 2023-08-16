#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "../utility.h"

enum class CardinalDirType
{
    North,
    East,
    South,
    West
};

enum class ActionType
{
    North,
    East,
    South,
    West,
    Left,
    Right,
    Forward
};

struct CardinalDir
{
    CardinalDirType cardinal_dir;
    int degrees;
    void turn_degrees(ActionType action, int val);
};

struct Action
{
    ActionType action_type;
    int val;
};

struct FerryState
{
    CardinalDir dir;
    int east_steps;
    int north_steps;
};

struct FerryState2
{
    int waypoint_east_steps;
    int waypoint_north_steps;
    int east_steps;
    int north_steps;
    void rotate_waypoint(ActionType action, int val);
    int get_waypoint_quadrant(int turn_degrees);
};

struct FerryNavigator
{
    FerryState state{ CardinalDirType::East, 0, 0 };
    void execute_actions(const std::vector<Action> &action_vec);
    void move_action(const Action &action);
};

/*
Used to solve part 2 of day 12, where we mostly move the waypoint
*/
struct FerryNavigator2
{
    FerryState2 state{ 10, 1, 0, 0 };

    void execute_actions(const std::vector<Action> &action_vec);
    void move_action(const Action &action);
};

const std::map<int,CardinalDirType> degree_dir_map{ {0,CardinalDirType::East},{90,CardinalDirType::North}, 
                                                    {180,CardinalDirType::West},{270,CardinalDirType::South} };

const std::map<char, ActionType> ActionMap{ {'E',ActionType::East},{'N',ActionType::North},{'W',ActionType::West},
                                            {'S',ActionType::South},{'L',ActionType::Left},{'R',ActionType::Right}, 
                                            {'F',ActionType::Forward} };

const std::map<CardinalDirType,ActionType> CardinalDirActionMap{ {CardinalDirType::East,ActionType::East }, {CardinalDirType::North,ActionType::North },
                                                                {CardinalDirType::West,ActionType::West },{CardinalDirType::South,ActionType::South } };

std::vector<Action> read_actions_from_file(const std::string &file_path);

int sol_12_1(const std::string &file_path)
{
    std::vector<Action> action_vec = read_actions_from_file(file_path);

    FerryNavigator navi;
    navi.execute_actions(action_vec);

    return std::abs(navi.state.east_steps) + std::abs(navi.state.north_steps);
}

int sol_12_2(const std::string &file_path)
{
    std::vector<Action> action_vec = read_actions_from_file(file_path);

    FerryNavigator2 navi;
    navi.execute_actions(action_vec);

    return std::abs(navi.state.east_steps) + std::abs(navi.state.north_steps);
}

void FerryNavigator::move_action(const Action &action)
{
    switch (action.action_type)
    {
    case ActionType::East:
        state.east_steps += action.val;
        break;
    case ActionType::North:
        state.north_steps += action.val;
        break;
    case ActionType::West:
        state.east_steps -= action.val;
        break;
    case ActionType::South:
        state.north_steps -= action.val;
        break;
    default:
        break;
    }
}

/* Quddrants:
2 | 1
-----
3 | 4
*/
int FerryState2::get_waypoint_quadrant(int turn_degrees)
{
    auto angle = std::atan2(waypoint_north_steps, waypoint_east_steps);
    angle += (M_PI / 180.0) * turn_degrees;
    if (angle <= -M_PI) angle += 2*M_PI;
    if (angle > M_PI) angle -= 2*M_PI;

    if (M_PI >= angle && angle > M_PI_2) return 2;
    if (M_PI_2 >= angle && angle > 0) return 1;
    if (0 >= angle && angle > -M_PI_2) return 4;
    if (-M_PI_2 >= angle && angle > -M_PI) return 3;
    throw std::runtime_error("Unable to calc waypoint quadrant");
}
/* 
Rotation is done by calculating the angle after rotation and detecting the quadrant
of the new waypoint. Afterwards the sign can be set based on quadrant 
*/
void FerryState2::rotate_waypoint(ActionType action, int val)
{
    if (ActionType::Right == action) val *= -1;
    int w_quadrant = get_waypoint_quadrant(val);
    int num_90_turns = val / 90;
    if (num_90_turns % 2) {
        int tmp = waypoint_east_steps;
        waypoint_east_steps = abs(waypoint_north_steps);
        waypoint_north_steps = abs(tmp);
    }
    else{
        waypoint_east_steps = abs(waypoint_east_steps);
        waypoint_north_steps = abs(waypoint_north_steps);
    }
    if (2 == w_quadrant) waypoint_east_steps *= -1;
    if (3 == w_quadrant) {
        waypoint_east_steps *= -1;
        waypoint_north_steps *= -1;
    }
    if (4 == w_quadrant) waypoint_north_steps *= -1;
}

void FerryNavigator2::move_action(const Action &action)
{
    switch (action.action_type)
    {
    case ActionType::East:
        state.waypoint_east_steps += action.val;
        break;
    case ActionType::North:
        state.waypoint_north_steps += action.val;
        break;
    case ActionType::West:
        state.waypoint_east_steps -= action.val;
        break;
    case ActionType::South:
        state.waypoint_north_steps -= action.val;
        break;
    default:
        break;
    }
}
void FerryNavigator2::execute_actions(const std::vector<Action> &action_vec)
{
    for (const auto &action : action_vec)
    {
        if (action.action_type == ActionType::East || action.action_type == ActionType::North || 
            action.action_type == ActionType::West || action.action_type == ActionType::South)
        {
            move_action(action);
        }
        else
        {
            if (action.action_type == ActionType::Left || action.action_type == ActionType::Right)
            {
                state.rotate_waypoint(action.action_type,action.val);
            }
            else
            {
                state.east_steps += action.val * state.waypoint_east_steps;
                state.north_steps += action.val * state.waypoint_north_steps;
            }
        }
    }
}

void FerryNavigator::execute_actions(const std::vector<Action> &action_vec)
{
    for (const auto &action : action_vec)
    {
        if (action.action_type == ActionType::East || action.action_type == ActionType::North || 
            action.action_type == ActionType::West || action.action_type == ActionType::South)
        {
            move_action(action);
        }
        else
        {
            if (action.action_type == ActionType::Left || action.action_type == ActionType::Right)
            {
                state.dir.turn_degrees(action.action_type,action.val);
            }
            else
            {
                auto new_action = action;
                new_action.action_type = CardinalDirActionMap.at(state.dir.cardinal_dir);
                move_action(new_action);
            }
        }
    }
}

void CardinalDir::turn_degrees(ActionType action, int val)
{
    if (ActionType::Right == action) val *= -1;
    degrees = (degrees+val+360) % 360;
    cardinal_dir = degree_dir_map.at(degrees);
}

std::vector<Action> read_actions_from_file(const std::string &file_path)
{
    std::vector<Action> action_vec;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            Action new_action;
            new_action.action_type = ActionMap.at(input_line[0]);
            new_action.val = std::stoi(input_line.substr(1));
            action_vec.push_back(new_action);
        }
        input_file.close();   //close the file object.
    }
    return action_vec;
}