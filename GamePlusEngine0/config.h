#pragma once

#include <stdint.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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


typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
