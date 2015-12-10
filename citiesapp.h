#ifndef CITIESAPP_H
#define CITIESAPP_H
#include "types.h"
#include "gamebot.h"

class psSocket;

class CitiesApp
{
public:
    CitiesApp(ushort port);
    virtual ~CitiesApp(){}
    virtual void Start() = 0;
protected:
    ushort _port;
};

class ServerApp : public CitiesApp
{
public:
   ServerApp();
   ServerApp(ushort port);
   virtual void Start();
};

class ClientApp : public CitiesApp
{
public:
    ClientApp();
    ClientApp(const std::string& host, ushort port);
   virtual void Start();
private:
    std::string _host;
};

#endif // CITIESAPP_H
