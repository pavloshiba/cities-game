#include "StdAfx.h"

#include <iostream>
#include "pssocket.h"
#include "citiesapp.h"

using namespace std;

#define SERVER_APP  1

void showOptions()
{
    cout << "1 - start server\n";
    cout << "else number - start client\n";

}

int main(int argc, char* argv[])
{
    CitiesApp* game;
    int appOption = 0;

    showOptions();

    cin >> appOption;
    cin.ignore();

    if (SERVER_APP == appOption)
        game = new ServerApp;
    else
        game = new ClientApp;

    game->Start();

    return 0;
}

