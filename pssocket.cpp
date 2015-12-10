#include "StdAfx.h"
#include "pssocket.h"

#include "platform.h"
#include "asserts.h"


//#define SOCKET_DEBUG
#ifdef SOCKET_DEBUG
#	include <iomanip.h>
#endif


static const char szEMPTY_ADRESS[] = "";
const int BUFF_SIZE = 24;


#ifdef WINDOWS
#	include <Ws2tcpip.h>
#	pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;

bool g_bWsaInitialized;

unsigned char  bLow = 2;
unsigned char  bHigh = 2;

void init_socket()
{
    if (!g_bWsaInitialized)
    {
        WSADATA wdData;

        int nResult = WSAStartup(MAKEWORD(bLow,bHigh), &wdData);
        ASSERTE(nResult == 0);

        g_bWsaInitialized = true;
    }
}

#else // LINUX
//#	include <sys/types.h>
#	include <unistd.h>
#	include <sys/socket.h>
#	include <errno.h>
#	include <signal.h>
#	include <string.h>
#	include <netdb.h>
#	include <arpa/inet.h>
#	include <errno.h>

#	define SOCKET_ERROR (-1)

enum { INVALID_SOCKET = -1 };

void init_socket() { }

void IgnoreSigpipe()
{
    signal(SIGPIPE, SIG_IGN);
}

int closesocket(SOCKET socket)
{
    shutdown(socket, SHUT_RDWR);
    return ::close(socket);
}
#endif


const char *SOCKET_ERROR_ARRAY[SE__MAX] =
{
    "",	//SE_NO_ERROR,

    "Unable to resolve name",			//SE_UNRESOLVED_NAME,
    "Incorrect ip address",				//SE_INVALID_IP,

    "Unable to create socket",			//SE_CREATE_SOCKET_ERROR,
    "Unable to establish connection",	//SE_CONNECTION_ERROR,
    "Unable to bind port",				//SE_BIND_ERROR,
    "Unable to listen port",			//SE_LISTEN_ERROR,
};


const char *GetErrorDescription(ESOCKETERRROR error)
{
    ASSERT(IN_RANGE(error, SE__MIN, SE__MAX));
    return SOCKET_ERROR_ARRAY[error];
}

psSocket::psSocket()
    : _Socket(0)
    , _Connected(false)
    , _Error(SE_NO_ERROR)
{
    init_socket();

    memset(&_LocalAddr, 0, sizeof(_LocalAddr));
    memset(&_RemoteAddr, 0, sizeof(_RemoteAddr));
}

psSocket::~psSocket()
{
}


/*static */
int psSocket::GetLastError()
{
#ifdef WINDOWS
    return WSAGetLastError();
#else
    return errno;
#endif
}

/*static */
string psSocket::GetIpByName(const string &sHostName)
{
    ASSERTE(!sHostName.empty());
    hostent* pHost = gethostbyname(sHostName.c_str());

    if (pHost)
    {
        return inet_ntoa(*(in_addr *)*pHost->h_addr_list);
    }
    else if (IpValid(sHostName))
    {
        return sHostName;
    }
    else
    {
        //LOG_DEBUG("Invalid host address : %s", sHostName.c_str());
        return "";
    }
}

/*static */
bool psSocket::IpValid(const string &ip)
{
    return inet_addr(ip.c_str()) != INADDR_NONE;
}


bool psSocket::connect(const string &sHostName, ushort usPort)
{
    ASSERTE(!sHostName.empty() && usPort > 0);
    bool bResult = false;

    do
    {
        string sAddress = GetIpByName(sHostName);
        if (sAddress.empty())
        {
            _Error = SE_UNRESOLVED_NAME;
            return false;
        }

        _LocalAddr.sin_family = AF_INET;
        _LocalAddr.sin_addr.s_addr = inet_addr(sAddress.c_str());
        _LocalAddr.sin_port = htons(usPort);

        if (INVALID_SOCKET == (_Socket = socket(AF_INET, SOCK_STREAM, 0)))
        {
            _Error = SE_CREATE_SOCKET_ERROR;
            break;
        }

        if (SOCKET_ERROR == ::connect(_Socket, (sockaddr *)&_LocalAddr, sizeof(_LocalAddr)))
        {
            _Error = SE_CONNECTION_ERROR;
            break;
        }

        bResult = _Connected = true;
    }
    while (false);

    return bResult;
}


bool psSocket::listen(ushort usPort, long lAddress /*= INADDR_ANY*/, int nMaxClient /*= MAX_CLIENT_QUEUE_SIZE*/)
{
    ASSERTE(usPort > 0);
    bool bResult = false;

    do
    {
        _LocalAddr.sin_addr.s_addr = htonl(lAddress);
        _LocalAddr.sin_port = htons(usPort);
        _LocalAddr.sin_family = AF_INET;

        if (INVALID_SOCKET == (_Socket = ::socket(AF_INET, SOCK_STREAM, 0)))
        {
            break;
        }

        int nOption = 1;
        if (SOCKET_ERROR == ::setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&nOption, sizeof(nOption)))
        {
            break;
        }

        if (SOCKET_ERROR == ::bind(_Socket, (struct sockaddr *)&_LocalAddr, sizeof(_LocalAddr)))
        {
            break;
        }

        if (SOCKET_ERROR == ::listen(_Socket, nMaxClient))
        {
            break;
        }

        bResult = true;
    }
    while (false);

    return bResult;
}

bool psSocket::listen(ushort usPort, const char* szAddress, int nMaxClient /*= MAXCONN*/)
{
    ASSERTE(szAddress && szAddress[0]);

    uint uAddress = ntohl(inet_addr(szAddress));
    return listen(usPort, uAddress, nMaxClient);
}

psSocket *psSocket::accept() const
{
    psSocket *psResultSocket = NULL;

    SOCKET clientSocket;
    socklen_t nAddressLen = sizeof(_RemoteAddr);
    if (INVALID_SOCKET == (clientSocket = ::accept(_Socket, (struct sockaddr *)&_RemoteAddr, &nAddressLen)))
    {
        return NULL;
    }

    if (!(psResultSocket = new (nothrow) psSocket()))
    {
        closesocket(clientSocket);
        return NULL;
    }

    psResultSocket->_Socket = clientSocket;
    psResultSocket->_LocalAddr = _LocalAddr;
    psResultSocket->_RemoteAddr = _RemoteAddr;
    psResultSocket->_Connected = true;

    return psResultSocket;
}


bool psSocket::shutdown(int nMode /*= SD_BOTH*/)
{
    _Connected = false;
    return ::shutdown(_Socket, nMode) == 0;
}

bool psSocket::close()
{
    int res = 0;

    if (_Socket != INVALID_SOCKET)
    {
        res = ::closesocket(_Socket);
    }

    _Connected = false;
    return res == 0;
}



#ifdef SOCKET_DEBUG
#	include <iostream>
#	define DUMP_LEN 30

void dump_buffer(const char *buffer, int length)
{
    int len = length > DUMP_LEN ? DUMP_LEN : length;
    for (int i = 0; i < len; ++i)
    {
        cout << (hex) << setw(2) << setfill('0') << (int)buffer[i] << ' ';
    }
    if (length > len) cout << "..";
    cout << endl;
}

#endif


long psSocket::send(void *data, int nLength)
{
    ASSERTE(data && nLength > 0);

    long lBytesSend = 0; // assume the value 0 means error
    const char *data_ptr = (const char *)data;

    do
    {
        int nSendResult = ::send(_Socket, data_ptr, nLength, 0);
#ifdef SOCKET_DEBUG
        cout << nSendResult << " bytes send -- ";
        dump_buffer(data_ptr, nSendResult);
#endif
        if (SOCKET_ERROR == nSendResult)
        {
            int last_error = GetLastError();
#ifdef WINDOWS
            if (WSA_IO_PENDING != last_error)
#else // LINUX
            if (EAGAIN != last_error)
#endif
            {
                //cout << "NOT WAS_IO_PENDING" << endl;
            }
#ifdef SOCKET_DEBUG
            else
            {
                cout << "WSA IOPENDING" << endl;
            }
#endif // SOCKET_DEBUG

            lBytesSend = 0;
            break;
        }

        lBytesSend += nSendResult;
        data_ptr += nSendResult;
        nLength -= nSendResult;
    }
    while (nLength);

    return lBytesSend;
}

long psSocket::recv(void *buffer, int nLength)
{
    ASSERTE(buffer && nLength > 0);

    long lBytesRecv = 0;
    char *buffer_ptr = (char *)buffer;

    do
    {
        int nRecvResult = ::recv(_Socket, buffer_ptr, nLength, 0);
#ifdef SOCKET_DEBUG
        cout << nRecvResult << " bytes recv -- ";
        dump_buffer(buffer_ptr, nRecvResult);
#endif
        if (SOCKET_ERROR == nRecvResult)
        {
            int last_error = GetLastError();
#ifdef WINDOWS
            if (WSA_IO_PENDING != last_error)
#else // LINUX
            if (EAGAIN != last_error)
#endif
            {
                //cout << "NOT WAS_IO_PENDING" << endl;
            }
#ifdef SOCKET_DEBUG
            else
            {
                cout << "WSA IOPENDING" << endl;
            }
#endif // SOCKET_DEBUG

            lBytesRecv = 0;
            break;
        }

        if (0 == nRecvResult) // connection was gracefully closed
        {
            break;
        }

        lBytesRecv += nRecvResult;
        buffer_ptr += nRecvResult;
        nLength -= nRecvResult;
    }
    while (false);

    return lBytesRecv;
}


ushort psSocket::getLocalPort() const
{
    sockaddr saPeer;
    socklen_t nPeerLen = sizeof(saPeer);
    return getsockname(_Socket, &saPeer, &nPeerLen) ? 0 : ntohs(((sockaddr_in *)&saPeer)->sin_port);
}

ushort psSocket::getRemotePort() const
{
    ASSERTE(_Connected == true);
    sockaddr saPeer;
    socklen_t nPeerLen = sizeof(saPeer);
    return getpeername(_Socket, &saPeer, &nPeerLen) ? 0 : ntohs(((sockaddr_in *)&saPeer)->sin_port);
}


const char *psSocket::getLocalAddress() const
{
    ASSERTE(_Socket);

    sockaddr saPeer;
    socklen_t nPeerLen  = sizeof(saPeer);
    return getsockname(_Socket, &saPeer, &nPeerLen) ? szEMPTY_ADRESS : inet_ntoa(((sockaddr_in *)&saPeer)->sin_addr);
}

const char *psSocket::getRemoteAddress() const
{
    ASSERTE(_Connected == true);

    sockaddr saPeer;
    socklen_t nPeerLen  = sizeof(saPeer);
    return getpeername(_Socket, &saPeer, &nPeerLen) ? szEMPTY_ADRESS : inet_ntoa(((sockaddr_in *)&saPeer)->sin_addr);
}


uint psSocket::getRaddr() const
{
    return (uint)_RemoteAddr.sin_addr.s_addr;
}

uint psSocket::getLaddr() const
{
    return (uint)_LocalAddr.sin_addr.s_addr;
}


string psSocket::getLocalEndpoint() const
{
    char buffer[BUFF_SIZE];
    sprintf(buffer, "%s:%u", getLocalAddress(), getLocalPort());
    return buffer;
}

string psSocket::getRemoteEndpoint() const
{
    char buffer[BUFF_SIZE];
    sprintf(buffer, "%s:%u", getRemoteAddress(), getRemotePort());
    return buffer;
}


bool psSocket::isConnected() const
{
    return _Connected;
}

ESOCKETERRROR psSocket::getError() const
{
    return _Error;
}
