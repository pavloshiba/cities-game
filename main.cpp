#include "citiesapp.h"
#include <iostream>

using std::cin;
using std::cout;

void ShowAppOptions()
{
    cout << "1 - start server\n";
    cout << "else - start client\n";

}

int main()
{
    CitiesApp* game;
    int appOption = 0;

    ShowAppOptions();

    cin >> appOption;
    cin.ignore();

    if (SERVER_APP == appOption)
        game = new ServerApp;
    else
        game = new ClientApp;

    game->Start();

    return 0;
}

