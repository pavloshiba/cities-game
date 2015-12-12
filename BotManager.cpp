#include "BotManager.h"

GameBot* BotManager::getBot(int level)
{
    GameBot* bot;
    
    switch(level)
    {
    default:
    case 0:
        bot = new LowBot;
        break;
    case 1:
        bot = new MediumBot;
        break;
    case 2:
        bot = new HardBot;
        break;
        bot = new HardBot;
        break;
    };

    return bot;
}