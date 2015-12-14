#ifndef _BOTS_H_
#define _BOTS_H

#include "gamebot.h"

/* bot that check player answer using own database*/
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
};

class HardBot : public DontTrustrulBot
{
public:
    HardBot();
};

#endif
