#include <unordered_map>
#include <string>

#include "../utility.h"

using Passport = std::unordered_map<std::string, std::string>;

const std::vector<std::string> MANDATORY_FIELDS{ "byr","iyr","eyr","hgt","hcl","ecl","pid"};
const std::vector<std::string> OPTIONAL_FIELDS{ "cid" };
const std::vector<std::string> EYECOLOR_VALUES{ "amb","blu","brn","gry","grn","hzl","oth"};

std::vector<Passport> read_passports_from_file(const std::string& file_path);
bool is_passport_valid_1(const Passport &passP, const std::vector<std::string> &mand, const std::vector<std::string> &opt);
bool is_passport_valid_2(const Passport &passP);

int sol_4_1(const std::string &file_path)
{
    std::vector<Passport> passports = read_passports_from_file(file_path);
    int num_valid_passwords{ 0 };

    for (const auto & pp : passports)
    {
        if (is_passport_valid_1(pp, MANDATORY_FIELDS, OPTIONAL_FIELDS)) ++num_valid_passwords; 
    }

    return num_valid_passwords;
}


int sol_4_2(const std::string &file_path)
{
    std::vector<Passport> passports = read_passports_from_file(file_path);
    int num_valid_passwords{ 0 };

    for (const auto & pp : passports)
    {
        if (is_passport_valid_1(pp, MANDATORY_FIELDS, OPTIONAL_FIELDS) && is_passport_valid_2(pp)) ++num_valid_passwords; 
    }

    return num_valid_passwords;
}

bool year_check(std::string val, int len, int min, int max)
{
    int year = std::stoi(val);
    if (val.length() != len || year < min || year > max) return false;
    return true;
}

bool height_check(std::string val)
{
    // hgt (Height) - a number followed by either cm or in:
    //     If cm, the number must be at least 150 and at most 193.
    //     If in, the number must be at least 59 and at most 76.
    std::string ending = val.substr(val.length()-2);
    if (ending == "cm")
    {
        int height = std::stoi(val.substr(0,val.length()-2));
        if (height < 150 || height > 193) return false;
    }
    else
    {
        if (ending == "in")
        {
            int height = std::stoi(val.substr(0,val.length()-2));
            if (height < 59 || height > 76) return false;
        }
        else return false;
    }
    return true;
}

bool eye_color_check(const std::vector<std::string> &valid_values, std::string val)
{
    for (auto &valid : valid_values)
    {
        if (valid == val) return true;
    }
    return false;
}

bool is_passport_valid_2(const Passport &passP)
{
    std::string cur = passP.at("byr");
    // byr (Birth Year) - four digits; at least 1920 and at most 2002.
    if (!year_check(cur,4,1920,2002)) return false;

    // iyr (Issue Year) - four digits; at least 2010 and at most 2020.
    cur = passP.at("iyr");
    if (!year_check(cur,4,2010,2020)) return false;


    // eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
    cur = passP.at("eyr");
    if (!year_check(cur,4,2020,2030)) return false;

    cur = passP.at("hgt");
    if (!height_check(cur)) return false;

    // hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
    cur = passP.at("hcl");
    if (cur[0] != '#' || cur.length() != 7) return false;
    for (auto c : cur.substr(1))
    {
        if (!((c >= '0' && c <= '9') || ( c >= 'a' && c<='f'))) return false;
    }

    // ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
    cur = passP.at("ecl");
    if(!eye_color_check(EYECOLOR_VALUES, cur)) return false;

    // pid (Passport ID) - a nine-digit number, including leading zeroes.
    cur = passP.at("pid");
    if (cur.length() != 9) return false;
    for (auto c : cur)
    {
        if (c < '0' || c > '9') return false;
    }

    return true;
}
bool is_passport_valid_1(const Passport &passP, const std::vector<std::string> &mand, const std::vector<std::string> &opt)
{
    for (const auto &entry : mand)
    {
        if (passP.find(entry) == passP.end()) return false;
    }

    return true;
}

std::vector<Passport> read_passports_from_file(const std::string& file_path)
{
    std::vector<Passport> p_p_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        Passport new_passport;
        while(getline(input_file, input_line))
        {  
            if (input_line == "")
            {
                p_p_vec.push_back(new_passport);
                new_passport.clear();
                continue;
            }
            std::vector<std::string> line_split = split_string(input_line, " ");
            for (auto &entry : line_split)
            {
                std::vector<std::string> entry_split = split_string(entry, ":");
                new_passport[entry_split[0]] = entry_split[1];
            }
        }
        p_p_vec.push_back(new_passport);
        input_file.close();   
    }
    return p_p_vec;
}

