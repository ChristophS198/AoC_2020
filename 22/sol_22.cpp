#include <string>
#include <vector>
#include <queue>

#include "../utility.h"

using CardDeck = std::queue<int>;
using GameScore = uint64_t;

std::vector<CardDeck> get_card_decks_from_data(const std::string &file_path);
void play_single_round(CardDeck &player1, CardDeck &player2);
GameScore play_single_game(CardDeck &player1, CardDeck &player2);
GameScore calc_score(CardDeck &card_deck);

int sol_22_1(const std::string &file_path)
{
    std::vector<CardDeck> card_decks = get_card_decks_from_data(file_path);



    return play_single_game(card_decks[0], card_decks[1]);
}

int sol_22_2(const std::string &file_path)
{

    return 0;
}

GameScore calc_score(CardDeck &card_deck)
{
    auto deck_size = card_deck.size();
    GameScore score{ 0 };
    for (;deck_size>0; --deck_size)
    {
        score += card_deck.front()*deck_size;
        card_deck.pop();
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
    player1.pop();
    int card_player2 = player2.front();
    player2.pop();

    if (card_player1 > card_player2)
    {
        player1.push(card_player1);
        player1.push(card_player2);
    }
    else
    {
        if (card_player2 > card_player1)
        {
            player2.push(card_player2);
            player2.push(card_player1);
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
                    card_deck_vec.back().push(card_val);
                }
            }
        }
        input_file.close();   
    }
    return card_deck_vec;
}