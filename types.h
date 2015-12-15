#ifndef __TYPES__
#define __TYPES__

#include "platform.h"

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::nothrow;

#ifdef WINDOWS

#	include <windows.h>
#	define ALIGNED(n) __declspec(align(n))

typedef __int64 int64;
typedef unsigned __int64 uint64;

#elif defined(LINUX)

#	include <inttypes.h>

//typedef int64_t int64;
typedef long long int int64;
//typedef uint64_t uint64;
typedef unsigned long long int uint64;

#endif


#define BITS_IN_BYTE 8

#define DEFAULT_APP_PORT        1993
#define DEFAULT_APP_HOST        "localhost"

#define SERVER_APP  1

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned char uchar;


typedef vector<std::string> string_v;
typedef vector<string_v> string_vv;

/*typedef vector<std::wstring> wstring_v;
typedef vector<wstring_v> wstring_vv;
*/
#endif // __TYPES__
