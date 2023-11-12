#include <string>
#include <vector>
#include <deque> // using deque in order to traverse the queue
#include <unordered_set>

#include "../utility.h"

using CardDeck = std::deque<int>;
using GameScore = uint64_t;

struct CardDeckState
{
    CardDeck player1;
    CardDeck player2;
};

/**
 * @brief corresponds to a set where the entries must be unique for each state of card decks
 * one easy solution is to convert each card deck to strings and prepend the size
 * 
 */
class CardDeckMemory
{
    public:
        bool insert_card_deck_state(const CardDeckState &state);

    private:
        std::unordered_set<std::string> memory;
};

std::vector<CardDeck> get_card_decks_from_data(const std::string &file_path);
void play_single_round(CardDeck &player1, CardDeck &player2);
GameScore play_single_game(CardDeck &player1, CardDeck &player2);
GameScore calc_score(CardDeck &card_deck);
void play_rec_combat_game(CardDeck &player1, CardDeck &player2);
void play_single_round_rec_combat(CardDeckMemory &deck_mem, CardDeck &player1, CardDeck &player2);


int sol_22_1(const std::string &file_path)
{
    std::vector<CardDeck> card_decks = get_card_decks_from_data(file_path);

    return play_single_game(card_decks[0], card_decks[1]);
}

int sol_22_2(const std::string &file_path)
{
    std::vector<CardDeck> card_decks = get_card_decks_from_data(file_path);
    auto player1 = card_decks[0];
    auto player2 = card_decks[1];
    play_rec_combat_game(player1, player2);

    if (player1.empty())
    {
        return calc_score(player2);
    }
    else
    {
        return calc_score(player1);
    }
}

void play_rec_combat_game(CardDeck &player1, CardDeck &player2)
{
    CardDeckMemory deck_memory; // each game or sub-game has its own deck_memory

    while (!player1.empty() && !player2.empty())
    {
        play_single_round_rec_combat(deck_memory, player1, player2);
    }
}

void play_single_round_rec_combat(CardDeckMemory &deck_mem, CardDeck &player1, CardDeck &player2)
{
    if (player1.empty() || player2.empty())
    {
        return;
    }

    if (!deck_mem.insert_card_deck_state(CardDeckState{player1,player2})) 
    {
        // game ends and player1 wins (is achieved by emptying player2s deck)
        player2.clear();
        return;
    }
    else
    {
        // each player draws the next card
        int card_player1 = player1.front();
        player1.pop_front();
        int card_player2 = player2.front();
        player2.pop_front();

        // If both players have at least as many cards remaining in their deck as the value 
        // of the card they just drew, the winner of the round is determined by playing a new game of Recursive Combat
        if (card_player1 <= player1.size() && card_player2 <= player2.size())
        {
            CardDeck sub_game_pl1{player1.begin(), player1.begin()+card_player1};
            CardDeck sub_game_pl2{player2.begin(), player2.begin()+card_player2};
            play_rec_combat_game(sub_game_pl1, sub_game_pl2);
            if (sub_game_pl1.empty())
            {
                player2.push_back(card_player2);
                player2.push_back(card_player1);
            }
            else
            {
                player1.push_back(card_player1);
                player1.push_back(card_player2);
            }
        }
        else
        {
            // find winner based on card values
            if (card_player1 > card_player2)
            {
                player1.push_back(card_player1);
                player1.push_back(card_player2);
            }
            else
            {
                if (card_player2 > card_player1)
                {
                    player2.push_back(card_player2);
                    player2.push_back(card_player1);
                }
                else
                {
                    throw std::runtime_error("play_single_round: both cards have equal values!");
                }
            }

        }
    }
}

// if CardDeckState already exists in memory no insertion is done and false returned
// otherwise true is returned and insertion is executed
bool CardDeckMemory::insert_card_deck_state(const CardDeckState &state)
{
    std::string mem_key;
    mem_key += "size:" + std::to_string(state.player1.size()) + ";";
    for (auto card_it=state.player1.begin(); card_it != state.player1.end(); ++card_it)
    {
        mem_key += std::to_string(*card_it) + ";";
    }

    mem_key += "size:" + std::to_string(state.player2.size()) + ";";
    for (auto card_it=state.player2.begin(); card_it != state.player2.end(); ++card_it)
    {
        mem_key += std::to_string(*card_it) + ";";
    }

    // try to insert mem_key in memory and return whether insertion was executed or not
     auto res = memory.insert(mem_key).second;
     return res;
}

GameScore calc_score(CardDeck &card_deck)
{
    auto deck_size = card_deck.size();
    GameScore score{ 0 };
    for (;deck_size>0; --deck_size)
    {
        score += card_deck.front()*deck_size;
        card_deck.pop_front();
    }

    return score;
}

GameScore play_single_game(CardDeck &player1, CardDeck &player2)
{
    while (!player1.empty() && !player2.empty())
    {
        play_single_round(player1, player2);
    }

    if (player1.empty())
    {
        return calc_score(player2);
    }
    else
    {
        return calc_score(player1);
    }
}

void play_single_round(CardDeck &player1, CardDeck &player2)
{
    if (player1.empty() || player2.empty())
    {
        return;
    }

    int card_player1 = player1.front();
    player1.pop_front();
    int card_player2 = player2.front();
    player2.pop_front();

    if (card_player1 > card_player2)
    {
        player1.push_back(card_player1);
        player1.push_back(card_player2);
    }
    else
    {
        if (card_player2 > card_player1)
        {
            player2.push_back(card_player2);
            player2.push_back(card_player1);
        }
        else
        {
            throw std::runtime_error("play_single_round: both cards have equal values!");
        }
    }

}

std::vector<CardDeck> get_card_decks_from_data(const std::string &file_path)
{
    std::vector<CardDeck> card_deck_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;

        while(getline(input_file, input_line)){
            std::vector<std::string> split_res = split_string(input_line," ");
            if (input_line.length() == 0) 
            {
                continue;
            }
            else
            {
                if (split_res[0] == "Player")
                {
                    card_deck_vec.push_back(CardDeck());
                }
                else
                {
                    int card_val = convert_to_num<int>(split_res[0]);
                    card_deck_vec.back().push_back(card_val);
                }
            }
        }
        input_file.close();   
    }
    return card_deck_vec;
}