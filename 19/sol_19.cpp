#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>

#include "../utility.h"

using TMath = int64_t;

std::pair<TMath,size_t> convert_to_num(const std::string c_vec, size_t pos);
size_t skip_space(const std::string &expr, size_t pos);
TMath eval_operator(char op, TMath op1, TMath op2);
std::pair<TMath,size_t> eval_expr(const std::string &expr, size_t pos, TMath intermediate_sum, bool invert_op_precedence=false);


TMath sol_19_1(const std::string &file_path)
{
    std::vector<std::string> math_expressions = read_string_vec_from_file(file_path);
    TMath sum{ 0 };

    for (auto &expr : math_expressions)
    {
        std::pair<TMath,size_t> res = eval_expr(expr, 0,0);
        sum += res.first;
    }

    return sum;
}


TMath sol_19_2(const std::string &file_path)
{
    std::vector<std::string> math_expressions = read_string_vec_from_file(file_path);
    TMath sum{ 0 };

    for (auto &expr : math_expressions)
    {
        std::pair<TMath,size_t> res = eval_expr(expr, 0,0, true);
        sum += res.first;
    }

    return sum;
}

std::pair<TMath,size_t> eval_expr(const std::string &expr, size_t pos, TMath intermediate_sum, bool invert_op_precedence)
{
    char op{ '+' };

    pos = skip_space(expr,pos); // move pos to next char != space 
    std::pair<TMath,size_t> tmp_dig{0,0};

    while (pos < expr.length())
    {
        char c_1{ expr.at(pos) };

        if (std::isdigit(static_cast<unsigned char>(c_1)))
        {
            tmp_dig = convert_to_num(expr,pos);
            pos = tmp_dig.second;
            intermediate_sum = eval_operator(op, intermediate_sum, tmp_dig.first);
        }
        else
        {
            switch (c_1)
            {
            case '+':
                // update operator 
                op = expr.at(pos);
                ++pos;
                break;
            case '*':
                // update operator 
                op = expr.at(pos);
                ++pos;
                if (invert_op_precedence)
                {
                    tmp_dig = eval_expr(expr,pos,0, invert_op_precedence);
                    pos = tmp_dig.second;
                    intermediate_sum = eval_operator(op, intermediate_sum, tmp_dig.first);
                    return { intermediate_sum, pos };
                }
                break;
            case '(':
                ++pos;
                tmp_dig = eval_expr(expr,pos,0, invert_op_precedence);
                pos = tmp_dig.second;
                intermediate_sum = eval_operator(op, intermediate_sum, tmp_dig.first);
                break;
            case ')':
                ++pos;
                return { intermediate_sum,pos };
                break;
            default:
                std::ostringstream err_msg; 
                err_msg << "eval_expr found an invalid character " << c_1 << " at pos" << pos << "!";
                throw std::invalid_argument(err_msg.str());
                break;
            }

        }
        pos = skip_space(expr,pos); // move pos to next char != space 
    }
    
    return { intermediate_sum,pos };
}

TMath eval_operator(char op, TMath op1, TMath op2)
{
    switch (op)
    {
    case '+':
        return op1 + op2;
        break;
    case '*':
        return op1 * op2;
        break;
    
    default:
        std::ostringstream err_msg; 
        err_msg << "eval_operator called with invalid operator" << op << "!";
        throw std::invalid_argument(err_msg.str());
        return -1;
        break;
    }
}

size_t skip_space(const std::string &expr, size_t pos)
{
    while (pos < expr.length() && expr.at(pos)==' ')
    {
        ++pos;
    }
    return pos;
}

std::pair<TMath,size_t> convert_to_num(const std::string c_vec, size_t pos)
{
    TMath num{};
    while (pos < c_vec.length() && std::isdigit(static_cast<unsigned char>(c_vec.at(pos))))
    {
        num = num*10 + c_vec.at(pos) - '0';
        ++pos;
    }
    return {num, pos};
}