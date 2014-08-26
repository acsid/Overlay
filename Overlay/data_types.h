#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <windows.h>

typedef unsigned char UInt8;
typedef signed char Int8;

typedef unsigned short int UInt16;
typedef signed short int Int16;

typedef unsigned long int UInt32;
typedef signed long int Int32;

typedef unsigned long long int UInt64;
typedef signed long long int Int64;

typedef UInt64 MaxUInt;
typedef Int64 MaxInt;

typedef bool (WINAPI * MODULE_SEARCH_ROUTINE)(bool & found);
typedef bool (WINAPI * MODULE_INIT_ROUTINE)();
typedef void (WINAPI * MODULE_CLEANUP_ROUTINE)();

#endif // DATA_TYPES_H
