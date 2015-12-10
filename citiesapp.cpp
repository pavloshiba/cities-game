#include "citiesapp.h"
#include "pssocket.h"
#include "macroses.h"

#include <thread>

using std::thread;
using std::cout;
using std::cin;
using std::endl;

#include <cstring>
using std::memset;


#define END_GAME_CHAR '-'

#define PLAYER_INFO_MESSAGE(x)  (std::string(BOT_FORB_MESAGE) + \
                                 std::string(" You have ") + \
                                 std::to_string(x) + \
                                 std::string(" tries!"))

#define SERVER_WIN_SIGNAL  BOT_WIN
#define SERVER_WIN_MESSAGE (std::string(BOT_WIN_MESSAGE) + std::string(OVER_MESSAGE))

#define SERVER_LOSE_SIGNAL BOT_LOSE
#define SERVER_LOSE_MESSAGE (std::string(BOT_LOSE_MESAGE) + std::string(OVER_MESSAGE))

const int BUFF_SIZE = 127;

const char START_MESSAGE[] = "Hello! Let's play!('-' to surrender)";
const char OVER_MESSAGE[] =  "Game over! Buy!";


void ServerApp::Start()
{
    psSocket sock;

    cout << "Server started....\n";

    sock.listen(APP_PORT);

    
    while(psSocket* client = sock.accept())
    {

        if (client != NULL)
        {
            const char* clientLocAddr;
            char recievingData[BUFF_SIZE];
            char lastChar = 0;

            memset(recievingData,0,ARRAY_SIZE(recievingData));

            //PRE GAME PROCEDURE
            clientLocAddr = client->getLocalAddress();

            cout << "Client connected: " << clientLocAddr << endl;

            BOT_LEVEL botLevel;
            client->recv(&botLevel, sizeof(botLevel));

            GameBot bot(botLevel);
            bool isEnd = false;

            std::string responce = bot.getRandomCity();
            lastChar = responce.back();

            client->send((void*)responce.c_str(),responce.size());

            do
            {
                // BEGIN PLAY
                if (client->recv(recievingData, BUFF_SIZE) != 0)
                {

                    cout << "Player " << clientLocAddr << " sad: " << recievingData << endl;
                   
                    responce = bot.getResponse(recievingData);

                    if (BOT_WIN == responce)
                    {
                        responce = SERVER_WIN_SIGNAL;
                        isEnd = true;
                    }
                    else if (BOT_LOSE == responce)
                    {
                        responce = SERVER_LOSE_SIGNAL;
                        isEnd = true;
                    }
                    else if (BOT_FORB == responce)
                    {
                        responce = PLAYER_INFO_MESSAGE(bot.getTries());
                    }                  
                    else
                        lastChar = responce.back();

                    } // TODO: debug
                
                    
                client->send((void*)responce.c_str(),BUFF_SIZE);

                if (isEnd)
                {
                    client->close();
                    sock.shutdown();
                    break;
                }

            } while (true);
    } // end client
       // else cout << "Cannot accept client\n";
    }
    
}

void ClientApp::Start()
{
    char replay = 'n';
    do
    {
        psSocket sock;
        char recievingData[BUFF_SIZE];
        std::string responce = "";

        memset(recievingData,0,ARRAY_SIZE(recievingData));

        cout << "Start connecting to " << APP_HOST << " " << APP_PORT << endl;

        if (sock.connect(APP_HOST,APP_PORT))
        {
            cout << "Connected....\n";
            cout << "Choose game difficulty(0 - easy, 1 - medium, 2 - hard)\n";
            int botLevel;
            cin >> botLevel;
            cin.ignore();

            sock.send((void*)&botLevel,sizeof(botLevel));
            cout << START_MESSAGE << endl;

            do
            {

                if (sock.recv(recievingData, BUFF_SIZE) != 0)
                {
                    if (END_GAME_CHAR == responce[0])
                    {
                        cout << OVER_MESSAGE << endl;
                        break;
                    }
                    std::string recievingStr(recievingData);
                    
                    if (SERVER_LOSE_SIGNAL == recievingStr)
                    {
                        cout << SERVER_LOSE_MESSAGE << endl;
                        break;
                    }
                    else if (SERVER_WIN_SIGNAL == recievingStr)
                    {
                        cout << SERVER_WIN_MESSAGE << endl;
                        break;
                    }

                    cout << "Server sad: "<< recievingData << endl;
                    cout << "your answer >:";

                    std::getline(std::cin,responce);

                    sock.send((void*)responce.c_str(),BUFF_SIZE);

                } else break;

            } while(true);
        }

        sock.close();
        cout << "Would you like to replay?(y/n)";

    }while (cin >> replay && replay == 'y');
}



/*
                if ((END_GAME_CHAR == recievingData[0]) || (PLAYER_TRIES == 0))
                {
                    if (client->send((void*)SERVER_WIN_SIGNAL, sizeof(SERVER_WIN_SIGNAL)) != 0)
                    {
                        cout << "Player " << clientLocAddr << " over the game!" << endl;
                        client->close();
                        sock.shutdown();
                        break;
                    }

                }

                if (recievingData[0] == lastChar) // first player's symbol same as last  symbol
                {
                    responce = bot.getResponse(recievingData);

                    if (BOT_LOSE == responce)
                    {
                        client->send((void*)SERVER_LOSE_SIGNAL,sizeof(SERVER_LOSE_SIGNAL));
                        client->close();
                        sock.shutdown();
                        break;
                    }

                    lastChar = responce.back();
                    client->send((void*)responce.c_str(),BUFF_SIZE);

                } // TODO: debug
                else
                {
                    --PLAYER_TRIES;
                    client->send((void*)PLAYER_INFO_MESSAGE.c_str(),BUFF_SIZE);

                }

*/
