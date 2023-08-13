#include <map>
#include <string>
#include <vector>

#include "../utility.h"

enum class BSInsType
{
    ACC,
    JMP,
    NOP
};

const std::map<std::string, BSInsType> INS_MAP{ {"acc", BSInsType::ACC}, 
                                                {"jmp", BSInsType::JMP},
                                                {"nop", BSInsType::NOP} };

struct BSOperation
{
    BSInsType type;
    int val;
};

struct BootCodeOperator
{
    int accumulator{ 0 };
    int cur_index{ 0 };
    std::pair<int,bool> execute_ins_till_cycle(const std::vector<BSOperation> &op_vec, int man_idx=-1);
    int find_correct_accumulator(const std::vector<BSOperation> &op_vec);
    void execute_bs_op(const BSOperation &op);
};

std::vector<BSOperation> get_bs_operations(const std::string &file_path);

int sol_8_1(const std::string &file_path)
{
    std::vector<BSOperation> op_vec = get_bs_operations(file_path);
    BootCodeOperator boot_code_operator;

    return boot_code_operator.execute_ins_till_cycle(op_vec).first;
}


int sol_8_2(const std::string &file_path)
{
    std::vector<BSOperation> op_vec = get_bs_operations(file_path);
    BootCodeOperator boot_code_operator;

    return boot_code_operator.find_correct_accumulator(op_vec);
}

/*
Idea is to choose each line once and if it is of type jmp or nop switch to the other type + 
execute the instruction set with switched type
*/
int BootCodeOperator::find_correct_accumulator(const std::vector<BSOperation> &op_vec)
{

    for (int i=0; i<op_vec.size(); ++i)
    {
        // reset values
        cur_index = 0;
        accumulator = 0;

        if (BSInsType::ACC == op_vec[i].type) continue;
        else
        {
            auto res = execute_ins_till_cycle(op_vec,i);
            if (res.second) return res.first;
        }
    }
    return -1;
}


void BootCodeOperator::execute_bs_op(const BSOperation &op)
{
    switch(op.type)
    {
        case BSInsType::ACC:
            accumulator += op.val;
            ++cur_index;
            break;
        case BSInsType::JMP:
            cur_index += op.val;
            break;
        case BSInsType::NOP:
            ++cur_index;
            break;
    }
}

std::pair<int,bool> BootCodeOperator::execute_ins_till_cycle(const std::vector<BSOperation> &op_vec, int man_idx)
{
    std::vector<bool> op_executed(op_vec.size(), 0); // marks which operations have been executed already

    while (cur_index < op_vec.size() && 0 == op_executed[cur_index])
    {
        BSOperation cur_op = op_vec[cur_index]; // copy of current operation
        op_executed[cur_index] = 1;
        if (cur_index == man_idx) // if index position matches man_idx, switch between nop and jmp
        {
            if (cur_op.type == BSInsType::JMP) cur_op.type = BSInsType::NOP; 
            else if (cur_op.type == BSInsType::NOP) cur_op.type = BSInsType::JMP; 
        }
        execute_bs_op(cur_op);
    }
    bool end_reached = cur_index == op_vec.size();
    return { accumulator, end_reached };
}

std::vector<BSOperation> get_bs_operations(const std::string &file_path)
{
    std::vector<BSOperation> op_vec;
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            auto split_input = split_string(input_line, " ");
            BSInsType op_type = INS_MAP.at(split_input[0]);
            int op_val = std::stoi(split_input[1]);
            op_vec.push_back({ op_type, op_val });
        }
        input_file.close();   
    }
    return op_vec;
}