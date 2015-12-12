#include "gamebot.h"
#include "cities.h"
#include <algorithm>
#include <ctime>

using std::string;

// incorrect cities name symbols
const string INCORRECT_CHARACTERS = "0123456789-=`!@#$%^&*()_+-[]\'/|\\\",.<> ";

/*
 * The number of possible wrong answers for
 * the player depending on the complexity of the game
*/
#define PLAYER_TRIES_LOW    12
#define PLAYER_TRIES_MEDIUM 6
#define PLAYER_TRIES_HARD   3

GameBot::GameBot()
{
}

GameBot::~GameBot() {}

ushort GameBot::getTries() const
{ 
    return _playerTries;
}

string GameBot::getResponse(string &opponentAnswer)
{
    if (_playerTries == 1) return BOT_WIN;

    removeCharsFromString(opponentAnswer, INCORRECT_CHARACTERS);

    if ((opponentAnswer.size() == 0) || (opponentAnswer[0] != _lastChar))
    {
        --_playerTries;
        return BOT_FORB;
    }
    else
    {
        auto it = find(_forbiddenCities.begin(),_forbiddenCities.end(),opponentAnswer);

        if( !(it == _forbiddenCities.end()))
        {
            --_playerTries;
            return BOT_FORB;
        }
        else
        {

            _forbiddenCities.push_back(opponentAnswer);

            // know possible cities that not forbiden

            string_v knowingCities = Cities::byFirstChar(_knowingCities,opponentAnswer.back());

            string_v possibleAnswers(knowingCities.size());

            Cities::citySort(knowingCities);
            Cities::citySort(_forbiddenCities);

            it = set_difference(knowingCities.begin(),knowingCities.end(),
                                             _forbiddenCities.begin(),_forbiddenCities.end(),
                                             possibleAnswers.begin());

            possibleAnswers.resize(it - possibleAnswers.begin());



            if(possibleAnswers.size() == 0)
                return BOT_LOSE;
            else
            {
                _forbiddenCities.push_back(possibleAnswers.at(0));
                _lastChar = possibleAnswers.at(0).at(0);

                return possibleAnswers.at(0);
            }
            
        }
    }

}

string GameBot::getResponse(char *opponentAnswer)
{
    string oponentAnswerStr(opponentAnswer);
    if (oponentAnswerStr.size() != 0)
        return this->getResponse(oponentAnswerStr);
    else 
    {
        --_playerTries;
        return BOT_FORB;
    }
}

string GameBot::getRandomCity()
{
    srand((unsigned)time(0));
    int randIndex = rand()%_knowingCities.size() + 1;

    std::string result = _knowingCities.at(randIndex);
    _lastChar = result.back();

    return result;
}

void GameBot::reset()
{
    _forbiddenCities.clear();
}

void GameBot::initBot(int botFactor)
{
    _forbiddenCities = string_v(0);
    Cities::initData(CITY_DATA_FILE,_knowingCities);
    random_shuffle(_knowingCities.begin(),_knowingCities.end());

    int eraseCount = _knowingCities.size() - (int)_knowingCities.size()/botFactor;

    _knowingCities.erase(_knowingCities.begin(), _knowingCities.begin() + eraseCount);
    Cities::citySort(_knowingCities);
}

void GameBot::removeCharsFromString(string &str, const string& charsToRemove)
{
   for (size_t i = 0; i < charsToRemove.size(); ++i )
   {
      str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
   }
}

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




