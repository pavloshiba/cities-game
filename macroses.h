#ifndef __H_MACROES__
#define __H_MACROES__

#define INVALID_RESULT		-1

#ifdef DEBUG_INFO_SHOWTIMER
#	define START_TIMER(x) int64 x = GetTickCount64();
#	define SHOW_TIME(x) cout << GetTickCount64() - x << endl;
#else
#	define START_TIMER(x)
#	define SHOW_TIME(x)
#endif

#define ARRAY_SIZE(A) ( sizeof(A) / sizeof((A)[0]) )

#define _MIN(a,b) (((a) > (b)) ? (b) : (a))
#define _MAX(a,b) (((a) < (b)) ? (b) : (a))

#define IN_RANGE(Value, Min, Max) ((size_t)((size_t)(Value) - (size_t)(Min)) < (size_t)((size_t)(Max) - (size_t)(Min)))

#define ALIGN_UP(Size, Alignment)	(((size_t)(Size) + ((Alignment) - 1)) & ~(size_t)((Alignment) - 1))

#endif // __H_MACROES__
