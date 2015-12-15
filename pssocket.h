#ifndef __XSOCKET_H_INCLUDED
#define __XSOCKET_H_INCLUDED

#include "types.h"
#ifdef WINDOWS
#	include <winsock2.h>
#else
#	include <netinet/in.h>
//#	define SD_BOTH SHUT_RDWR

// A flag that describes what types of operation will no longer be allowed.
// Shutdown receive operations & shutdown send operations
#	define SD_BOTH 2
    typedef int SOCKET;

    void IgnoreSigpipe();
#endif

#define MAX_CLIENT_QUEUE_SIZE	5

enum ESOCKETERRROR
{
    SE__MIN,
    SE_NO_ERROR = SE__MIN,

    SE_UNRESOLVED_NAME,
    SE_INVALID_IP,

    SE_CREATE_SOCKET_ERROR,
    SE_CONNECTION_ERROR,
    SE_BIND_ERROR,
    SE_LISTEN_ERROR,

    SE__MAX,
};


const char *GetErrorDescription(ESOCKETERRROR error);


/*
 * psSocket - pavlo shiba's (ps) cross-platform socket wrapper.
*/
class psSocket
{
public:
    psSocket();
    ~psSocket();

public:
    static int GetLastError();
    static string GetIpByName(const string &sHostName);
    static bool isIpValid(const string &ip);

public:
    bool connect(const string &sHostName, ushort usPort);

    bool listen(ushort usPort, long lAddress = INADDR_ANY, int nMaxClient = MAX_CLIENT_QUEUE_SIZE);
    bool listen(ushort usPort, const char* szAddress, int nMaxClient /*= MAXCONN*/);
    psSocket *accept() const;

    bool shutdown(int nMode = SD_BOTH);
    bool close();

    long send(void *data, int nLength);
    long recv(void *buffer, int nLength);

public:
    ushort getLocalPort() const;
    ushort getRemotePort() const;

    const char *getLocalAddress() const;
    const char *getRemoteAddress() const;

    uint getRaddr() const;
    uint getLaddr() const;

    string getLocalEndpoint() const;
    string getRemoteEndpoint() const;

    bool isConnected() const;
    ESOCKETERRROR getError() const;

private:
    struct sockaddr_in	LocalAddr_;
    struct sockaddr_in	RemoteAddr_;
    SOCKET              Socket_;
    bool                Connected_;

    ESOCKETERRROR       Error_;
};

#endif // __XSOCKET_H_INCLUDED
