#ifndef GAMEBOT_H
#define GAMEBOT_H


#include "types.h"

#define BOT_WIN  ">"
#define BOT_LOSE "<"
#define BOT_FORB "!"


#define BOT_WIN_MESSAGE "I\'m WIN!"
#define BOT_LOSE_MESAGE "I\'m LOSE!"
#define BOT_FORB_MESAGE "THIS NAME ILLEGAL!"

enum class BOT_LEVEL
{
    LOW = 0,
    MEDIUM,
    HIGH
};


/// Bot doesn't check player's answer - trustful bot
class GameBot
{
public:
    explicit GameBot(BOT_LEVEL level = BOT_LEVEL::LOW);

    string    getResponse(string &opponentAnswer);
    string    getResponse(char* opponentAnswer);
    string    getRandomCity();
    BOT_LEVEL getLevel() const;
    ushort    getTries() const;

    void      setLevel(BOT_LEVEL level);
    void      reset();

protected:
    void      initBot();
    void      initTries();
    void      removeCharsFromString(string &str, const string &charsToRemove );

protected:
    string_v  _knowingCities;
    string_v  _forbiddenCities;
    BOT_LEVEL _level;
    ushort    _playerTries;
    char      _lastChar;

};

#endif // GAMEBOT_H
