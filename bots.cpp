#include "bots.h"
#include <algorithm>

LowBot::LowBot()
{
    int lowBotFactor = 12;
    GameBot::initBot(lowBotFactor);
    playerTries_= PLAYER_TRIES_LOW;
}

MediumBot::MediumBot()
{
    int mediumBotFactor = 5;
    GameBot::initBot(mediumBotFactor);
    playerTries_= PLAYER_TRIES_MEDIUM;
}

HardBot::HardBot()
{
    GameBot::initBot(1);
    playerTries_= PLAYER_TRIES_HARD;
}

DontTrustrulBot::DontTrustrulBot() {}

bool DontTrustrulBot::checkAnswer(const string& answer) const
{
    bool res = false;

    auto it = std::find(knowingCities_.begin(),knowingCities_.end(), answer);

    if(it != knowingCities_.end())
        res = true;

    return res;
}

/*virtual*/
string DontTrustrulBot::getResponse(string& opponentAnswer)
{
    string res;

    if (checkAnswer(opponentAnswer))
        res = GameBot::getResponse(opponentAnswer);
    else
    {
        --playerTries_;
        if (playerTries_== 1)
            res = BOT_WIN;
        else
            res = BOT_FORB;
    }

    return res;
}

