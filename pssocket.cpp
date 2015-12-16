#include "pssocket.h"

#include "platform.h"
#include "asserts.h"
#include <stdio.h>

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

bool g_bWsaInitialized =false;

unsigned char  bLow = 2;
unsigned char  bHigh = 2;

void initSocket_()
{
    if (!g_bWsaInitialized)
    {
        WSADATA wdData;

        int nResult = WSAStartup(MAKEWORD(bLow,bHigh), &wdData);
        if (nResult != 0)
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

void initSocket_() { }

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


const char *SOCKETError__ARRAY[SE__MAX] =
{
    "",	//SE_NOError_,

    "Unable to resolve name",			//SE_UNRESOLVED_NAME,
    "Incorrect ip address",				//SE_INVALID_IP,

    "Unable to create socket",			//SE_CREATESocket_Error_,
    "Unable to establish connection",	//SE_CONNECTIONError_,
    "Unable to bind port",				//SE_BINDError_,
    "Unable to listen port",			//SE_LISTENError_,
};


const char *GetErrorDescription(ESOCKETERRROR error)
{
    ASSERT(IN_RANGE(error, SE__MIN, SE__MAX));
    return SOCKETError__ARRAY[error];
}

psSocket::psSocket()
    : Socket_(0)
    , Connected_(false)
    , Error_(SE_NO_ERROR)
{
    initSocket_();

    memset(&LocalAddr_, 0, sizeof(LocalAddr_));
    memset(&RemoteAddr_, 0, sizeof(RemoteAddr_));
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
    else if (isIpValid(sHostName))
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
bool psSocket::isIpValid(const string &ip)
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
            Error_ = SE_UNRESOLVED_NAME;
            return false;
        }

        LocalAddr_.sin_family = AF_INET;
        LocalAddr_.sin_addr.s_addr = inet_addr(sAddress.c_str());
        LocalAddr_.sin_port = htons(usPort);

        if (INVALID_SOCKET == (Socket_ = socket(AF_INET, SOCK_STREAM, 0)))
        {
            Error_ = SE_CREATE_SOCKET_ERROR;
            break;
        }

        if (SOCKET_ERROR == ::connect(Socket_, (sockaddr *)&LocalAddr_, sizeof(LocalAddr_)))
        {
            Error_ = SE_CONNECTION_ERROR;
            break;
        }

        bResult = Connected_ = true;
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
        LocalAddr_.sin_addr.s_addr = htonl(lAddress);
        LocalAddr_.sin_port = htons(usPort);
        LocalAddr_.sin_family = AF_INET;

        if (INVALID_SOCKET == (Socket_ = ::socket(AF_INET, SOCK_STREAM, 0)))
        {
            break;
        }

        int nOption = 1;
        if (SOCKET_ERROR == ::setsockopt(Socket_, SOL_SOCKET, SO_REUSEADDR, (char *)&nOption, sizeof(nOption)))
        {
            break;
        }

        if (SOCKET_ERROR == ::bind(Socket_, (struct sockaddr *)&LocalAddr_, sizeof(LocalAddr_)))
        {
            break;
        }

        if (SOCKET_ERROR == ::listen(Socket_, nMaxClient))
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
    socklen_t nAddressLen = sizeof(RemoteAddr_);
    if (INVALID_SOCKET == (clientSocket = ::accept(Socket_, (struct sockaddr *)&RemoteAddr_, &nAddressLen)))
    {
        return NULL;
    }

    if (!(psResultSocket = new (nothrow) psSocket()))
    {
        closesocket(clientSocket);
        return NULL;
    }

    psResultSocket->Socket_ = clientSocket;
    psResultSocket->LocalAddr_ = LocalAddr_;
    psResultSocket->RemoteAddr_ = RemoteAddr_;
    psResultSocket->Connected_ = true;

    return psResultSocket;
}


bool psSocket::shutdown(int nMode /*= SD_BOTH*/)
{
    Connected_ = false;
    return ::shutdown(Socket_, nMode) == 0;
}

bool psSocket::close()
{
    int res = 0;

    if (Socket_ != INVALID_SOCKET)
    {
        res = ::closesocket(Socket_);
    }

    Connected_ = false;
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
        int nSendResult = ::send(Socket_, data_ptr, nLength, 0);
#ifdef SOCKET_DEBUG
        cout << nSendResult << " bytes send -- ";
        dump_buffer(data_ptr, nSendResult);
#endif
        if (SOCKET_ERROR == nSendResult)
        {
            int lastError_ = GetLastError();
#ifdef WINDOWS
            if (WSA_IO_PENDING != lastError_)
#else // LINUX
            if (EAGAIN != lastError_)
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
        int nRecvResult = ::recv(Socket_, buffer_ptr, nLength, 0);
#ifdef SOCKET_DEBUG
        cout << nRecvResult << " bytes recv -- ";
        dump_buffer(buffer_ptr, nRecvResult);
#endif
        if (SOCKET_ERROR == nRecvResult)
        {
            int lastError_ = GetLastError();
#ifdef WINDOWS
            if (WSA_IO_PENDING != lastError_)
#else // LINUX
            if (EAGAIN != lastError_)
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
    return getsockname(Socket_, &saPeer, &nPeerLen) ? 0 : ntohs(((sockaddr_in *)&saPeer)->sin_port);
}

ushort psSocket::getRemotePort() const
{
    ASSERTE(Connected_ == true);
    sockaddr saPeer;
    socklen_t nPeerLen = sizeof(saPeer);
    return getpeername(Socket_, &saPeer, &nPeerLen) ? 0 : ntohs(((sockaddr_in *)&saPeer)->sin_port);
}


const char *psSocket::getLocalAddress() const
{
    ASSERTE(Socket_);

    sockaddr saPeer;
    socklen_t nPeerLen  = sizeof(saPeer);
    return getsockname(Socket_, &saPeer, &nPeerLen) ? szEMPTY_ADRESS : inet_ntoa(((sockaddr_in *)&saPeer)->sin_addr);
}

const char *psSocket::getRemoteAddress() const
{
    ASSERTE(Connected_ == true);

    sockaddr saPeer;
    socklen_t nPeerLen  = sizeof(saPeer);
    return getpeername(Socket_, &saPeer, &nPeerLen) ? szEMPTY_ADRESS : inet_ntoa(((sockaddr_in *)&saPeer)->sin_addr);
}


uint psSocket::getRaddr() const
{
    return (uint)RemoteAddr_.sin_addr.s_addr;
}

uint psSocket::getLaddr() const
{
    return (uint)LocalAddr_.sin_addr.s_addr;
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
    return Connected_;
}

ESOCKETERRROR psSocket::getError() const
{
    return Error_;
}
