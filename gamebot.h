#ifndef GAMEBOT_H
#define GAMEBOT_H


#include "types.h"

#define BOT_WIN  ">"
#define BOT_LOSE "<"
#define BOT_FORB "!"


#define BOT_WIN_MESSAGE "I\'m WIN!"
#define BOT_LOSE_MESAGE "I\'m LOSE!"
#define BOT_FORB_MESAGE "THIS NAME ILLEGAL!"


/// Bot doesn't check player's answer - trustful bot
class GameBot
{
public:
    virtual ~GameBot();
    
    virtual string getResponse(string &opponentAnswer);

    string getResponse(char* opponentAnswer);
    string getRandomCity();
    //BOT_LEVEL getLevel() const;
    ushort getTries() const;
    //void      setLevel(BOT_LEVEL level);
    void reset();

protected:
    GameBot();
    void initBot(int botFactor);
    void removeCharsFromString(string &str, const string &charsToRemove );

protected:
    string_v  _knowingCities;
    string_v  _forbiddenCities;
    //BOT_LEVEL _level;
    ushort    _playerTries;
    char      _lastChar;

};

class DontTrustrulBot : public GameBot
{
public:
    /*virtual */ 
    string getResponse(string &opponentAnswer);
protected:
    DontTrustrulBot();
    /*virtual*/
    bool checkAnswer(const string& answer) const;
};

class LowBot : public GameBot
{
public:
    LowBot();
};

class MediumBot : public DontTrustrulBot
{
public:
    MediumBot();
protected:
    /*virtual*/ 
    void initBot();
};

class HardBot : public DontTrustrulBot
{
public:
    HardBot();
};

#endif // GAMEBOT_H
