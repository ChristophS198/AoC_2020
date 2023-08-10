#include <unordered_map>
#include <string>

#include "../utility.h"


struct PasswordPolicy
{
    int minNumOcc{ 0 };
    int maxNumOcc{ 0 };
    char letter{ 'a' };
};
struct PasswordAndPolicy
{
    PasswordPolicy policy{ };
    std::string password{ };
    static bool is_password_valid_1(const PasswordAndPolicy &pAndp);
    static bool is_password_valid_2(const PasswordAndPolicy &pAndp);
};

std::vector<PasswordAndPolicy> read_data_from_file(const std::string& file_path);

int sol_2_1(const std::string &file_path)
{
    std::vector<PasswordAndPolicy> passwordsAndPolicies = read_data_from_file(file_path);
    int num_valid_passwords{ 0 };

    for (const auto & pp : passwordsAndPolicies)
    {
        if (PasswordAndPolicy::is_password_valid_1(pp)) ++num_valid_passwords; 
    }

    return num_valid_passwords;
}


int sol_2_2(const std::string &file_path)
{
    std::vector<PasswordAndPolicy> passwordsAndPolicies = read_data_from_file(file_path);
    int num_valid_passwords{ 0 };

    for (const auto & pp : passwordsAndPolicies)
    {
        if (PasswordAndPolicy::is_password_valid_2(pp)) ++num_valid_passwords; 
    }

    return num_valid_passwords;
}

bool PasswordAndPolicy::is_password_valid_2(const PasswordAndPolicy &pAndp)
{
    const auto & passw{ pAndp.password };
    bool first = pAndp.policy.letter == passw[pAndp.policy.minNumOcc-1];
    bool second = pAndp.policy.letter == passw[pAndp.policy.maxNumOcc-1];
    return first != second;
}
bool PasswordAndPolicy::is_password_valid_1(const PasswordAndPolicy &pAndp)
{
    auto n = std::count(pAndp.password.begin(), pAndp.password.end(), pAndp.policy.letter);

    return n >= pAndp.policy.minNumOcc && n <= pAndp.policy.maxNumOcc;
}

std::vector<PasswordAndPolicy> read_data_from_file(const std::string& file_path)
{
    std::vector<PasswordAndPolicy> p_p_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            std::vector<std::string> line_split = split_string(input_line, ":");
            std::vector<int> nums = parse_string_to_number_vec<int>(line_split[0]);
            PasswordPolicy policy{ nums[0], nums[1], line_split[0].back() };
            std::string pass{ line_split[1].substr(1) };
            PasswordAndPolicy new_p_p{ policy, pass };

            p_p_vec.push_back(new_p_p);
        }
        input_file.close();   
    }
    return p_p_vec;
}

