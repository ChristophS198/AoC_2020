#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <bitset>

#include "../utility.h"

using t36UInt = uint64_t;
using tMemory = std::unordered_map<t36UInt,t36UInt>;

struct StoreOp
{
    t36UInt mem_add;
    t36UInt val;
};

struct BitMask
{
    t36UInt one_mask;
    t36UInt zero_mask;
    std::vector<int> x_positions;
};

struct ProgramInstruction
{
    BitMask bit_mask;
    std::vector<StoreOp> store_ops;
};

std::vector<ProgramInstruction> read_program_ins_from_file(const std::string &file_path);
BitMask parse_bit_mask(const std::string &mask_str);
void print_mask(std::ostream &out, const BitMask &mask);
t36UInt init_program(const std::vector<ProgramInstruction> &program_ins_vec, tMemory &mem);
t36UInt apply_mask(const BitMask &mask, t36UInt val, bool apply_zero_mask=true);
std::vector<t36UInt> apply_mem_mask(BitMask mask, t36UInt mem_val);
t36UInt init_program_v2(const std::vector<ProgramInstruction> &program_ins_vec, tMemory &mem);

t36UInt sol_14_1(const std::string &file_path)
{
    std::vector<ProgramInstruction> program_ins = read_program_ins_from_file(file_path);
    tMemory mem;

    return init_program(program_ins, mem);
}


t36UInt sol_14_2(const std::string &file_path)
{
    std::vector<ProgramInstruction> program_ins = read_program_ins_from_file(file_path);
    tMemory mem;

    // too low: 3654995300986
    return init_program_v2(program_ins, mem);
}

t36UInt init_program_v2(const std::vector<ProgramInstruction> &program_ins_vec, tMemory &mem)
{

    for (const auto &ins_set : program_ins_vec)
    {
        for (const auto &store_op : ins_set.store_ops)
        {
            auto mem_locations = apply_mem_mask(ins_set.bit_mask, store_op.mem_add);
            for (auto mem_loc : mem_locations)
            {
                mem[mem_loc] = store_op.val;
            }
        }
    }
    t36UInt sum{ 0 };
    for (auto &mem_entry : mem) sum += mem_entry.second;
    return sum;
}

t36UInt init_program(const std::vector<ProgramInstruction> &program_ins_vec, tMemory &mem)
{

    for (const auto &ins_set : program_ins_vec)
    {
        for (const auto &store_op : ins_set.store_ops)
        {
            mem[store_op.mem_add] = apply_mask(ins_set.bit_mask, store_op.val);
        }
    }
    t36UInt sum{ 0 };
    for (auto &mem_entry : mem) sum += mem_entry.second;
    return sum;
}

std::vector<t36UInt> apply_mem_mask(BitMask mask, t36UInt mem_val)
{
    t36UInt mem_size = 1 << mask.x_positions.size(); // number of floating mem positions is defined by number of x: 2^X
    std::vector<t36UInt> mem_locations;
    mem_locations.reserve(mem_size); // reserve mem here so we can push_back during iterating over the same vector
    mem_locations.push_back(apply_mask(mask,mem_val, false));

    for (auto x_pos : mask.x_positions)
    {
        for (auto mem_loc : mem_locations)
        {
            t36UInt new_val = mem_loc ^ (1ull << x_pos); // flip bit at x_pos through xor
            mem_locations.push_back(new_val);
        }
    }
    if (mem_locations.size() != mem_locations.capacity()) throw std::runtime_error("Missing memory locations");
    return mem_locations;
}

t36UInt apply_mask(const BitMask &mask, t36UInt val, bool apply_zero_mask)
{
    val = val | mask.one_mask;
    if (apply_zero_mask) val = val & mask.zero_mask;

    return val;
}

BitMask parse_bit_mask(const std::string &mask_str)
{
    BitMask mask;
    mask.one_mask = 0;   // all zeroes
    mask.zero_mask = ~0; // all ones
    t36UInt bit_val{ 1 };
    int num_bits{ mask_str.size() - 1 };

    for (int i=num_bits; i>=0; --i)
    {
        if ('1' == mask_str[i]) mask.one_mask += bit_val;
        if ('0' == mask_str[i]) mask.zero_mask -= bit_val;
        if ('X' == mask_str[i]) mask.x_positions.push_back(num_bits-i);
        bit_val = bit_val << 1;
    }
    return mask;
}

std::vector<ProgramInstruction> read_program_ins_from_file(const std::string &file_path)
{
    std::vector<ProgramInstruction> program_ins_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        ProgramInstruction new_ins;
        while(getline(input_file, input_line)){  
            if ("mask" == input_line.substr(0,4))
            {
                if (new_ins.store_ops.size() > 0) program_ins_vec.push_back(new_ins);
                new_ins.store_ops.clear();
                new_ins.bit_mask = parse_bit_mask(input_line.substr(7));
            }
            else
            {
                std::vector<t36UInt> tmp = parse_string_to_number_vec<t36UInt>(input_line);
                new_ins.store_ops.push_back(StoreOp{tmp[0], tmp[1]});
            }
        }
        program_ins_vec.push_back(new_ins);
        input_file.close();   
    }
    return program_ins_vec;
}

void print_mask(std::ostream &out, const BitMask &mask)
{
    out << "zero mask = " << mask.zero_mask << "\n" << std::bitset<64>(mask.zero_mask) << "\n"; 
    out << "one mask = " << mask.one_mask << "\n" << std::bitset<64>(mask.one_mask) << "\n\n"; 
}