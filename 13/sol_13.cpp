#include <string>
#include <vector>
#include <map>
#include <limits>

#include "../utility.h"

struct InputData
{
    uint64_t earliest_time;
    std::vector<uint64_t> bus_ids;
};

uint64_t add_nxt_bus(uint64_t earliest_time, uint64_t offset, uint64_t nxt_bus_id, uint64_t inc_val);
InputData read_input_from_file(const std::string &file_path);

int sol_13_1(const std::string &file_path)
{
    InputData input_data = read_input_from_file(file_path);
    int min_wait_time_bus_id{ 0 };
    int min_wait_time{ std::numeric_limits<int>::max() };

    for (auto bus_id : input_data.bus_ids)
    {
        if (bus_id == -1) continue;

        auto cur_nxt_bus_wait_time = bus_id - input_data.earliest_time%bus_id;
        if (cur_nxt_bus_wait_time < min_wait_time)
        {
            min_wait_time = cur_nxt_bus_wait_time;
            min_wait_time_bus_id = bus_id;
        }
    }

    return min_wait_time_bus_id * min_wait_time;
}

/*
Idea is to always increment by a summand that is made up by multiplying all 
preceding bus IDs. This ensures that their relative offset (modulo) to the 
new value does not change. We increase until the relative offset of the new bus id
has the desired value 
*/
uint64_t sol_13_2(const std::string &file_path)
{
    InputData input_data = read_input_from_file(file_path);
    uint64_t earliest_time{ 0 };
    uint64_t inc_val{ 1 };

    for (int i=0; i<input_data.bus_ids.size(); ++i)
    {
        if (input_data.bus_ids[i] == -1) continue;
        earliest_time = add_nxt_bus(earliest_time, i, input_data.bus_ids[i], inc_val);
        inc_val *= input_data.bus_ids[i];
    }

    return earliest_time;
}

uint64_t add_nxt_bus(uint64_t earliest_time, uint64_t offset, uint64_t nxt_bus_id, uint64_t inc_val)
{
    uint64_t tmp_earliest_time{ earliest_time };
    offset = (nxt_bus_id - offset%nxt_bus_id) % nxt_bus_id; // change from time 
    while ((tmp_earliest_time % nxt_bus_id) != offset) tmp_earliest_time += inc_val;

    return tmp_earliest_time;
}

InputData read_input_from_file(const std::string &file_path)
{
    InputData input_data;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        // read first line
        getline(input_file, input_line);
        input_data.earliest_time = std::stoi(input_line);
        // read second line
        getline(input_file, input_line);
        auto bus_id_vec = split_string(input_line,",");
        for (auto bus_id : bus_id_vec)
        {
            if ("x" == bus_id) input_data.bus_ids.push_back(-1);
            else input_data.bus_ids.push_back(std::stoi(bus_id));
        }

        input_file.close();   //close the file object.
    }
    return input_data;
}