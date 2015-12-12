#ifndef _BOT_MANAGER_H_
#define _BOT_MANAGER_H_

#include "gamebot.h"

class BotManager
{
public:
    static GameBot* getBot(int level);
};

#endif

