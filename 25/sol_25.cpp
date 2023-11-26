#include <string>
#include <vector>
#include <set>

#include "../utility.h"

using TKey = std::uint64_t;
using LoopSize = std::uint32_t;

constexpr std::uint64_t SUBJ_NUM_DIV{ 20201227ull };

struct PublicKeys
{
    TKey cardKey;
    TKey doorKey;
};

PublicKeys get_public_keys(const std::string &file_path);
LoopSize calc_loop_size(TKey pub_key);
TKey transform_subject_num(LoopSize loop_size, TKey pub_key);

int sol_25_1(const std::string &file_path)
{
    PublicKeys pub_keys = get_public_keys(file_path);
    LoopSize card_loop_size = calc_loop_size(pub_keys.cardKey);
    LoopSize door_loop_size = calc_loop_size(pub_keys.doorKey);

    TKey card_encrpytion_key = transform_subject_num(card_loop_size, pub_keys.doorKey);
    TKey door_encrpytion_key = transform_subject_num(door_loop_size, pub_keys.cardKey);

    if (card_encrpytion_key != door_encrpytion_key)
    {
        throw std::runtime_error("sol_25_1: Failed to calc card or door encryption key!");
    }

    return door_encrpytion_key;
}

TKey transform_subject_num(LoopSize loop_size, TKey pub_key)
{
    TKey encryption_key{ 1 };

    for (LoopSize i=0; i<loop_size; ++i)
    {
        encryption_key *= pub_key;
        encryption_key %= SUBJ_NUM_DIV;
    }

    return encryption_key;
}

LoopSize calc_loop_size(TKey pub_key)
{
    std::uint64_t subject_num{ 7ull };
    std::uint64_t transformed_num{ 1ull };
    LoopSize loop_size{ 0 };

    while (transformed_num != pub_key)
    {
        transformed_num *= subject_num;
        transformed_num %= SUBJ_NUM_DIV;
        ++loop_size;
    }

    return loop_size;
}

PublicKeys get_public_keys(const std::string &file_path)
{
    PublicKeys keys;

    std::vector<TKey> key_vec = read_numbers_from_file<TKey>(file_path);
    keys.cardKey = key_vec[0];
    keys.doorKey = key_vec[1];

    return keys;
}