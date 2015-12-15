#ifndef GAMEBOT_H
#define GAMEBOT_H


#include "types.h"

#define BOT_WIN  ">"
#define BOT_LOSE "<"
#define BOT_FORB "!"

#define BOT_WIN_MESSAGE "I\'m WIN!"
#define BOT_LOSE_MESAGE "I\'m LOSE!"
#define BOT_FORB_MESAGE "THIS NAME ILLEGAL!"

/*
 * The number of possible wrong answers for
 * the player depending on the complexity of the game
*/
#define PLAYER_TRIES_LOW    12
#define PLAYER_TRIES_MEDIUM 6
#define PLAYER_TRIES_HARD   3

// incorrect cities name symbols
const string INCORRECT_CHARACTERS = "0123456789-=`!@#$%^&*()_+-[]\'/|\\\",.<> ";

/// Bot doesn't check player's answer - trustful bot
class GameBot
{
public:
    virtual ~GameBot();
    
    virtual string getResponse(string &opponentAnswer);

    string getResponse(char* opponentAnswer);
    string getRandomCity();
    ushort getTries() const;

    string_v getKnowingCities() const;
    string_v getForbiddenCities() const;

    void reset();

protected:
    GameBot();

    /*
    Initialize gamebot using CITY_DATA_FILE
    botFactor is value that define size of cities bot knowns
    */
    void initBot(int botFactor);

    void removeCharsFromString(string &str, const string &charsToRemove );

protected:
    string_v  knowingCities_;
    string_v  forbiddenCities_;
    ushort    playerTries_;
    char      lastChar_;
};

#endif // GAMEBOT_H
