#include "gamebot.h"
#include "cities.h"
#include <algorithm>
#include <ctime>

using std::string;

GameBot::GameBot(){}

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

