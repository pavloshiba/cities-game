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

GameBot::GameBot(BOT_LEVEL level)
    :_level(level)
{
    initBot();
    initTries();
}

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
        return getResponse(oponentAnswerStr);
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

BOT_LEVEL GameBot::getLevel() const
{
    return _level;
}

void GameBot::setLevel(BOT_LEVEL level)
{
    _level = level;
    initBot();
}

void GameBot::reset()
{
    _forbiddenCities.clear();
}

void GameBot::initBot()
{
    _forbiddenCities = string_v(0);
    Cities::initData(CITY_DATA_FILE,_knowingCities);

    int factor = 0;

    if (_level == BOT_LEVEL::HIGH)
        return;
    else if (_level == BOT_LEVEL::MEDIUM) // take away every 5th cities from _konwingCities
        factor = 5;
    else
        factor = 2; // bot level is LOW -  take away every 2nd cities from _konwingCities

    random_shuffle(_knowingCities.begin(),_knowingCities.end());

    factor = _knowingCities.size()/factor;

    _knowingCities.erase(_knowingCities.begin(), _knowingCities.begin() + factor);

    Cities::citySort(_knowingCities);
}

void GameBot::initTries()
{
   
    if (BOT_LEVEL::HIGH == _level)
        _playerTries = PLAYER_TRIES_HARD; // three tries for player with hard bot
    else if (BOT_LEVEL::MEDIUM == _level)
        _playerTries = PLAYER_TRIES_MEDIUM; // six tries for player with medium bot
    else
        _playerTries = PLAYER_TRIES_LOW;

}

void GameBot::removeCharsFromString(string &str, const string& charsToRemove)
{
   for (size_t i = 0; i < charsToRemove.size(); ++i )
   {
      str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
   }
}

