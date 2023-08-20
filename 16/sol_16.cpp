#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "../utility.h"

using FieldRange = std::vector<std::pair<int,int>>;

struct TicketField
{
    std::string field_name;
    FieldRange field_ranges;
};

struct STickets
{
    std::vector<int> my_ticket;
    std::vector<std::vector<int>> nearby_tickets;
};

using TicketDef = std::vector<TicketField>;

struct FieldCandidates
{
    std::vector<bool> field_pos;
    int num_possible_fields;
};
using PosCandidates = std::unordered_set<std::string>;

std::pair<TicketDef, STickets> get_ticket_data(const std::string &file_path);
std::vector<int> get_invalid_ticket_entries(const TicketDef &ticket_def, const std::vector<std::vector<int>> &nearby_tickets);
std::vector<int> get_out_of_range_entries(const TicketDef &ticket_def, const std::vector<int> &ticket);
bool num_in_range(int num, const  std::pair<int,int> &range);
bool num_in_field_range(int num, const FieldRange &field_range);
std::vector<std::vector<int>> get_valid_tickets(const TicketDef &ticket_def, const std::vector<std::vector<int>> &nearby_tickets);
std::unordered_map<std::string, int> get_assignment(const TicketDef &ticket_def, const std::vector<std::vector<int>> &nearby_tickets);

int sol_16_1(const std::string &file_path)
{
    std::pair<TicketDef, STickets> ticket_data = get_ticket_data(file_path);
    std::vector<int> invalid_entries = get_invalid_ticket_entries(ticket_data.first, ticket_data.second.nearby_tickets);

    int sum{ 0 };

    for (auto entry : invalid_entries) sum += entry;

    return sum;
}


int64_t sol_16_2(const std::string &file_path)
{
    std::pair<TicketDef, STickets> ticket_data = get_ticket_data(file_path);
    std::vector<std::vector<int>> valid_tickets = get_valid_tickets(ticket_data.first, ticket_data.second.nearby_tickets);
    std::unordered_map<std::string, int> assignment_map = get_assignment(ticket_data.first, valid_tickets);

    int64_t res{ 1 };
    for (auto &elem : assignment_map)
    {
        if ("departure" == split_string(elem.first," ")[0])
        {
            res *= ticket_data.second.my_ticket[elem.second];
        }
    }

    return res;
}

/*
Iterate through field_candidates_map and pos_candidates_vec and check for possible new assignments
if a new assignment is found, mark those entries as invalid in the other map/vec and repeat the check 
until no more new assignments can be found
*/
std::unordered_map<std::string,int> check_for_assignments(std::unordered_map<std::string, FieldCandidates> &field_candidates_map, 
                                                            std::vector<PosCandidates> &pos_candidates_vec, std::unordered_map<std::string, int> &assignment_map,
                                                                const std::unordered_set<int> &unassigend_ticket_pos, const std::unordered_set<std::string> &unassigend_ticket_fields)
{
    std::unordered_map<std::string,int> newly_assigned_fields;
    int assignment_ctr{ 1 };
    int num_ticket_fields = pos_candidates_vec.size();

    while (assignment_ctr > 0)
    {
        assignment_ctr = 0; // count assignments in each round

        // check field_candidates_map for possible new assignments
        for (auto &field_name : unassigend_ticket_fields)
        {
            FieldCandidates &cand = field_candidates_map[field_name];
            if (cand.num_possible_fields == 1)
            {
                if (auto search = assignment_map.find(field_name); search != assignment_map.end() )
                {
                    // assignemnt has already been handled in a previous round 
                    continue;
                }
                ++assignment_ctr;
                // only one possible field position remains -> assignment found
                for (int i=0; i<cand.field_pos.size(); ++i)
                {
                    // only one field_pos with true remains -> remove this assignment from unassigend_ticket_fields and unassigend_ticket_pos
                    if (cand.field_pos[i]) 
                    {
                        assignment_map[field_name] = i;
                        newly_assigned_fields[field_name] = i;
                        if (assignment_map.size() == num_ticket_fields) return newly_assigned_fields;
                        // set this position for remaining fields to false
                        for (auto &field_cand : field_candidates_map)
                        {
                            if (field_cand.first == field_name) continue;
                            if (true == field_cand.second.field_pos[i])
                            {
                                --field_cand.second.num_possible_fields;
                                field_cand.second.field_pos[i] = false;
                            }
                        }
                        break;
                    }
                }
            }
        }

        // check pos_candidates_vec for possible new assignments
        for (auto pos : unassigend_ticket_pos)
        {
            auto &pos_candidates_set = pos_candidates_vec[pos];
            if (pos_candidates_set.size() == 1)
            {
                std::string field_name_match = *pos_candidates_set.begin();
                if (auto search = assignment_map.find(field_name_match); search != assignment_map.end() )
                {
                    // assignemnt has already been handled in a previous round 
                    continue;
                }
                // since only one field_name remains for this position -> assignment found
                ++assignment_ctr;
                assignment_map[field_name_match] = pos;
                if (assignment_map.size() == num_ticket_fields) return newly_assigned_fields;
                newly_assigned_fields[field_name_match] = pos;
                // erase this field_name_match for all other positions 
                for (int j=0; j<pos_candidates_vec.size(); ++j)
                {
                    auto &pos_candidates = pos_candidates_vec[j];
                    if (j == pos) continue;
                    pos_candidates.erase(field_name_match);
                }
            }
        }
    }
    return newly_assigned_fields;
}

std::unordered_map<std::string, int> get_assignment(const TicketDef &ticket_def, const std::vector<std::vector<int>> &nearby_tickets)
{
    
    
    std::unordered_map<std::string, FieldCandidates> field_candidates_map;
    // std::vector<PosCandidates> pos_candidates_vec; // position value corresponds to index in vector
    std::unordered_map<std::string, TicketField> ticket_field_map;
    std::unordered_map<std::string, int> assignment_map;

    int num_ticket_fields{ ticket_def.size() };
    std::unordered_set<int> unassigend_ticket_pos; // vector of positions that are still unknown
    std::unordered_set<std::string> unassigend_ticket_fields; // vector of fields that are still unassigned
    // init candidates map so for each field each number position is valid
    int i{ 0 };
    for (const auto &ticket_field : ticket_def)
    {
        field_candidates_map[ticket_field.field_name] = { std::vector<bool>(num_ticket_fields, true), num_ticket_fields };
        ticket_field_map[ticket_field.field_name] = ticket_field;
        unassigend_ticket_pos.insert(i++);
        unassigend_ticket_fields.insert(ticket_field.field_name);
    }
    // vector to track which field names are still a valid match for a certain index position
    // vector index position corresponds to the field position in the ticket
    std::vector<PosCandidates> pos_candidates_vec(num_ticket_fields, unassigend_ticket_fields);

    for (const auto &ticket : nearby_tickets)
    {
        for (const auto &pos : unassigend_ticket_pos)
        {
            int num = ticket[pos];
            // iterate through all unassigned tickets fields and check if this number is invalid for any ticket field
            for (const auto &unassigend_ticket_field : unassigend_ticket_fields)
            {
                auto &ticket_field = ticket_field_map[unassigend_ticket_field];
                if (false == num_in_field_range(num, ticket_field.field_ranges))
                {
                    // denote current position as invalid for this field_name in field_candidates_map
                    FieldCandidates &cand = field_candidates_map[ticket_field.field_name];
                    if (true == cand.field_pos[pos])
                    {
                        cand.field_pos[pos] = false;
                        --cand.num_possible_fields;
                    }

                    // denote current field_name as invalid for this position in pos_candidates_vec
                    auto &pos_candidates_set = pos_candidates_vec[pos];
                    pos_candidates_set.erase(ticket_field.field_name); 
                }
            }
        }
        // check if any new assignments are possible
        std::unordered_map<std::string,int> newly_assigned_fields = check_for_assignments(field_candidates_map, 
                                                            pos_candidates_vec, assignment_map,
                                                                unassigend_ticket_pos, unassigend_ticket_fields);
        if (assignment_map.size() == num_ticket_fields) return assignment_map;
        // remove all newly found assignments from open positions and fields
        for (auto &assignment : newly_assigned_fields)
        {
            unassigend_ticket_fields.erase(assignment.first);
            unassigend_ticket_pos.erase(assignment.second);
        }
    }
    return assignment_map;
}

std::vector<std::vector<int>> get_valid_tickets(const TicketDef &ticket_def, const std::vector<std::vector<int>> &nearby_tickets)
{
    std::vector<std::vector<int>> valid_tickets;
    for (const auto &ticket : nearby_tickets)
    {
        std::vector<int> out_of_range_entries = get_out_of_range_entries(ticket_def, ticket);
        if (out_of_range_entries.size() == 0) valid_tickets.push_back(ticket);
    }

    return valid_tickets;
}

std::vector<int> get_out_of_range_entries(const TicketDef &ticket_def, const std::vector<int> &ticket)
{
    std::vector<int> invalid_entries;

    for (const auto &num : ticket)
    {
        bool is_valid = false;
        for (const auto &field : ticket_def)
        {
            for (const auto &range : field.field_ranges)
            {
                is_valid = is_valid || num_in_range(num,range);
            }
        }
        if (!is_valid) 
        {
            invalid_entries.push_back(num);
        }
    }
    return invalid_entries;
}

bool num_in_field_range(int num, const FieldRange &field_range)
{
    for (const auto &range : field_range) if (num_in_range(num, range)) return true;
    return false;
}

bool num_in_range(int num, const  std::pair<int,int> &range)
{
    if (num < range.first || num > range.second) return false;
    else return true;
}

std::vector<int> get_invalid_ticket_entries(const TicketDef &ticket_def, const std::vector<std::vector<int>> &nearby_tickets)
{
    std::vector<int> invalid_entries;

    for (const auto &ticket : nearby_tickets)
    {
        std::vector<int> out_of_range_entries = get_out_of_range_entries(ticket_def, ticket);
        invalid_entries.insert(invalid_entries.end(),out_of_range_entries.begin(), out_of_range_entries.end());
    }

    return invalid_entries;
}

TicketField parse_ticket_field_str(const std::string &field_str)
{
    TicketField ticket_field;

    std::vector<std::string> ticket_split = split_string(field_str, ": ");
    ticket_field.field_name = ticket_split[0];
    std::vector<std::string> range_split = split_string(ticket_split[1], " or ");

    for (auto &range : range_split)
    {
        std::vector<std::string> sub_range_split = split_string(range, "-");
        ticket_field.field_ranges.push_back({ std::stoi(sub_range_split[0]),std::stoi(sub_range_split[1]) });
    }
    return ticket_field;
}

std::pair<TicketDef, STickets> get_ticket_data(const std::string &file_path)
{
    TicketDef ticket_definition;
    STickets tickets;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        int parse_ctr{ 0 };
        while(getline(input_file, input_line)){  
            if ("" == input_line)
            {
                ++parse_ctr;
                continue;
            }
            else
            {
                if ("your ticket:" == input_line || "nearby tickets:" == input_line) continue;
                switch (parse_ctr)
                {
                case 0:
                    ticket_definition.push_back(parse_ticket_field_str(input_line));
                    break;
                case 1: // my ticket
                    tickets.my_ticket = parse_string_to_number_vec<int>(input_line);
                    break;
                case 2: // nearby ticket
                    tickets.nearby_tickets.push_back(parse_string_to_number_vec<int>(input_line));
                    break;
                default:
                    break;
                }
            }
        }
        input_file.close();
    }
    return {ticket_definition, tickets};
}