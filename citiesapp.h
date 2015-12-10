#ifndef CITIESAPP_H
#define CITIESAPP_H
#include "types.h"
#include "gamebot.h"

class psSocket;

class CitiesApp
{
public:
    virtual ~CitiesApp(){}

    virtual void Start() = 0;
};

class ServerApp : public CitiesApp
{
public:
   virtual void Start();
};

class ClientApp : public CitiesApp
{
public:
   virtual void Start();
};

#endif // CITIESAPP_H
