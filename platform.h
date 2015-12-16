#ifndef __PLATFORM_H_INCLUDED
#define __PLATFORM_H_INCLUDED


#if defined(WIN32) || defined(_WIN32) || defined (WIN64)

#	define WINDOWS
#	define WIN32_LEAN_AND_MEAN

#elif defined(__GNUC__)

#	define LINUX

#else

#	error Unsupported platform

#endif


#endif // __PLATFORM_H_INCLUDED
