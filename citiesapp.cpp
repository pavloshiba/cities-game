#include "citiesapp.h"
#include "pssocket.h"
#include "macroses.h"
#include "BotManager.h"

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

const char START_MESSAGE[] = "Hello! Let's play!(Press '-' to surrender)";
const char OVER_MESSAGE[] =  "Game over! Buy!";


CitiesApp::CitiesApp(ushort port) : port_(port)
{}

ServerApp::ServerApp()
    : CitiesApp(DEFAULT_APP_PORT)
{}

ServerApp::ServerApp(ushort port)
    : CitiesApp(port)
{}

void ServerApp::Start()
{
    psSocket mainSocket;

    cout << "Server started....\n";

    mainSocket.listen(port_);
    
    while(psSocket* client = mainSocket.accept())
    {

        if (client != NULL)
        {
            const char* clientLocAddr;
            char recievingData[BUFF_SIZE];

            memset(recievingData,0,ARRAY_SIZE(recievingData));

            //PRE GAME PROCEDURE
            clientLocAddr = client->getLocalAddress();

            cout << "Client connected: " << clientLocAddr;

            int botLevel;
            client->recv(&botLevel, sizeof(botLevel));

            GameBot* bot = BotManager::getBot(botLevel);
            bool isEnd = false;

            cout << ". Bot for new client created: level(" 
                << botLevel 
                << "), knowing cities - "
                << bot->getKnowingCities().size() << endl;
            
            std::string responce = bot->getRandomCity();

            client->send((void*)responce.c_str(),responce.size());

            do
            {
                // BEGIN PLAY
                if (client->recv(recievingData, BUFF_SIZE) != 0)
                {

                    cout << "Player " << clientLocAddr << " sad: " << recievingData << endl;
                   
                    responce = bot->getResponse(recievingData);

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
                        responce = PLAYER_INFO_MESSAGE(bot->getTries());
                    }      
                    
                  }
                  else break;                
                    
                client->send((void*)responce.c_str(),BUFF_SIZE);

                if (isEnd)
                {
                    client->close();
                    delete bot;
                    break;
                }

            } while (true);
         } // end client
       // else cout << "Cannot accept client\n";
    }
    
}

ClientApp::ClientApp()
    : CitiesApp(DEFAULT_APP_PORT),
      host_(DEFAULT_APP_HOST)
{}

ClientApp::ClientApp(const std::string &host, ushort port)
    : CitiesApp(port),
      host_(host)
{}

void ClientApp::Start()
{
    char replay = 'n';
    do
    {
        //cin.ignore();
        psSocket mainSocket;
        char recievingData[BUFF_SIZE];
        std::string responce = "";

        memset(recievingData,0,ARRAY_SIZE(recievingData));

        cout << "Start connecting to " << DEFAULT_APP_HOST << " " << DEFAULT_APP_PORT << endl;

        if (mainSocket.connect(host_, port_))
        {
            cout << "Connected....\n";
            cout << "Choose game difficulty(0 - easy, 1 - medium, 2 - hard)\n";

            int botLevel ;
            cin >> botLevel;
            cin.ignore();

            mainSocket.send((void*)&botLevel,sizeof(botLevel));
            cout << START_MESSAGE << endl;

            do
            {

                if (mainSocket.recv(recievingData, BUFF_SIZE) != 0)
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

                    mainSocket.send((void*)responce.c_str(),BUFF_SIZE);

                } else break;

            } while(true);
        }
        else
        {
            cout << "Cannot connect to server...." << endl;
            break;
        }

        mainSocket.close();
        cout << "Would you like to replay?(y/n)";

    } while (cin >> replay && cin.ignore() && replay == 'y');
}
