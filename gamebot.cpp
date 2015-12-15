#include "gamebot.h"
#include "cities.h"
#include <algorithm>
#include <ctime>

using std::string;

GameBot::GameBot(){}

GameBot::~GameBot() {}

ushort GameBot::getTries() const
{ 
    return playerTries_;
}

string GameBot::getResponse(string &opponentAnswer)
{
    string result = BOT_FORB;

    do
    {
        if (playerTries_ == 1) 
        { 
            result = BOT_WIN;
            break;
        }

        removeCharsFromString(opponentAnswer, INCORRECT_CHARACTERS);

        if ((opponentAnswer.size() == 0) || (opponentAnswer[0] != lastChar_))
        {
            --playerTries_;
            break;
        }
        else
        {
            auto it = find(forbiddenCities_.begin(),forbiddenCities_.end(),opponentAnswer);

            if( !(it == forbiddenCities_.end()))
            {
                --playerTries_;
                break;
            }
            else
            {

                forbiddenCities_.push_back(opponentAnswer);

                // know possible cities that not forbiden

                string_v knowingCities = Cities::citiesByFirstChar(knowingCities_,opponentAnswer.back());

                string_v possibleAnswers(knowingCities.size());

                Cities::citiesSort(knowingCities);
                Cities::citiesSort(forbiddenCities_);

                it = set_difference(knowingCities.begin(),knowingCities.end(),
                                                 forbiddenCities_.begin(),forbiddenCities_.end(),
                                                 possibleAnswers.begin());

                possibleAnswers.resize(it - possibleAnswers.begin());



                if(possibleAnswers.size() == 0)
                {
                    result = BOT_LOSE;
                    break;
                }
                else
                {
                    forbiddenCities_.push_back(possibleAnswers.at(0));
                    lastChar_ = possibleAnswers.at(0).at(0);
                    //TODO: remove from _knowingAnswers
                    result = possibleAnswers.at(0);
                    break;
                }
            
            }
        }
    } while(false);

    return result;

}

string GameBot::getResponse(char *opponentAnswer)
{
    string oponentAnswerStr(opponentAnswer);

    if (oponentAnswerStr.size() != 0)
        return this->getResponse(oponentAnswerStr);
    else 
    {
        --playerTries_;
        return BOT_FORB;
    }
}

string GameBot::getRandomCity()
{
    srand((unsigned)time(0));
    int randIndex = rand()%knowingCities_.size() + 1;

    std::string result = knowingCities_.at(randIndex);
    lastChar_ = result.back();

    return result;
}

string_v GameBot::getKnowingCities() const
{
    return knowingCities_;
}

string_v GameBot::getForbiddenCities() const
{
    return forbiddenCities_;
}

void GameBot::reset()
{
    forbiddenCities_.clear();
}


void GameBot::initBot(int botFactor)
{
    forbiddenCities_ = string_v(0);
    Cities::initData(CITY_DATA_FILE,knowingCities_);
    random_shuffle(knowingCities_.begin(),knowingCities_.end());

    int eraseCount = knowingCities_.size() - (int)knowingCities_.size()/botFactor;

    knowingCities_.erase(knowingCities_.begin(), knowingCities_.begin() + eraseCount);
    Cities::citiesSort(knowingCities_);
}

void GameBot::removeCharsFromString(string &str, const string& charsToRemove)
{
   for (size_t i = 0; i < charsToRemove.size(); ++i )
   {
      str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
   }
}

