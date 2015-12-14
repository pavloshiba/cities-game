#include "bots.h"

LowBot::LowBot()
{
    int lowBotFactor = 12;
    GameBot::initBot(lowBotFactor);
    _playerTries = PLAYER_TRIES_LOW;
}

MediumBot::MediumBot()
{
    int mediumBotFactor = 5;
    GameBot::initBot(mediumBotFactor);
    _playerTries = PLAYER_TRIES_MEDIUM;
}

HardBot::HardBot()
{
    GameBot::initBot(1);
    _playerTries = PLAYER_TRIES_HARD;
}

DontTrustrulBot::DontTrustrulBot() {}

bool DontTrustrulBot::checkAnswer(const string& answer) const
{
    bool res = false;

    auto it = std::find(_knowingCities.begin(),_knowingCities.end(), answer);

    if(it != _knowingCities.end())
        res = true;

    return res;
}

/*virtual*/
string DontTrustrulBot::getResponse(string& opponentAnswer)
{
    string res;
    if(checkAnswer(opponentAnswer))
        res = GameBot::getResponse(opponentAnswer);
    else
    {
        --_playerTries;
        if (_playerTries == 1)
            res = BOT_WIN;
        else
            res = BOT_FORB;
    }

    return res;
}

