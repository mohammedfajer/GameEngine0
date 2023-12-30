#pragma once

#include <stdint.h>

#define START_NAMESPACE namespace IceEngine {
#define END_NAMESPACE }

#define local_persist   static
#define global_variable static


//START_NAMESPACE
//#define local_persist   static
//#define global_variable static
//#define internal        static
//END_NAMESPACE

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i32 bool32;

typedef float real32;
typedef double real64;