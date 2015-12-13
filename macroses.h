#ifndef __H_MACROES__
#define __H_MACROES__

#define INVALID_RESULT		-1

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define ARRAY_SIZE(array) (sizeof(ArraySizeHelper(array)))

#define _MIN(a,b) (((a) > (b)) ? (b) : (a))
#define _MAX(a,b) (((a) < (b)) ? (b) : (a))

#define IN_RANGE(Value, Min, Max) ((size_t)((size_t)(Value) - (size_t)(Min)) < (size_t)((size_t)(Max) - (size_t)(Min)))

#define ALIGN_UP(Size, Alignment)	(((size_t)(Size) + ((Alignment) - 1)) & ~(size_t)((Alignment) - 1))

#endif // __H_MACROES__
