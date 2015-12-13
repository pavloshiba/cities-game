#include "BotManager.h"

enum BOT_LEVEL
{
    BOT_LOW_LEVEL = 0,
    BOT_MEDIUM_LEVEL,
    BOT_HIGH_LEVEL
};

GameBot* BotManager::getBot(int level)
{
    GameBot* bot;
    
    switch(level)
    {
    default:
    case BOT_LOW_LEVEL:
        bot = new LowBot;
        break;
    case BOT_MEDIUM_LEVEL:
        bot = new MediumBot;
        break;
    case BOT_HIGH_LEVEL:
        bot = new HardBot;
        break;
    };

    return bot;
}