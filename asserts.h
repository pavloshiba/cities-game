#ifndef __ASSERTS_H_INCLUDED
#define __ASSERTS_H_INCLUDED

#include "platform.h"
#include "macroses.h"


#ifdef _DEBUG
#	ifdef WINDOWS
#		define EXIT_PROCESS()	ExitProcess(-1);
#	else
#		include <unistd.h>
#		define EXIT_PROCESS()	_exit(-1);
#	endif
#else
#	define EXIT_PROCESS()
#endif


#ifdef WITH_ASSERTS
#	define ASSERT_STR(__COND__, __STR__) \
        if ( !(__COND__) ) { \
            LOG_FATAL(__STR__); \
            logger::finalize(); \
            EXIT_PROCESS(); \
        }
#	define ASSERTE(__COND__) \
        ASSERT_STR(__COND__, "----- \'" #__COND__ "\' condition failed -----")
#	define ASSERT(__COND__) \
    ASSERT_STR(__COND__, "----- \'" #__COND__ "\' condition failed -----")

#	define ASSERT_FAIL(__STR__) \
    ASSERT_STR(false, "----- " __STR__ " -----")

#	define ASSERT_NOT_IMPLEMENTED() \
    bool method_is_not_implemented = false; \
    ASSERT(method_is_not_implemented);

#else
#	define ASSERT_STR(__COND__, __STR__)
#	define ASSERTE(__COND__) __COND__
#	define ASSERT(__COND__) __COND__
#	define ASSERT_FAIL(__STR__)
#	define ASSERT_NOT_IMPLEMENTED()
#endif


#endif // __ASSERTS_H_INCLUDED
