// Math/Math_init.h

#pragma once

#ifndef MMYMYS_TOOLS_MATH_INIT_H
#define MMYMYS_TOOLS_MATH_INIT_H

#include <inttypes.h>
#include <stddef.h>

#include "../toolsfamily.h"

TOOLFUNC__ double toolSumImpl(const double* arr, size_t count) {
    if (!count) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += arr[i];
    }
    return sum;
}

TOOLFUNC__ double toolAverageImpl(const double* arr, size_t count) {
    if (!count) return 0.0;
    return toolSumImpl(arr, count) / (double)count;
}

TOOLFUNC__ int16_t toolCoordGetX16(int32_t packed) { return (int16_t)(packed & 0xFFFF); }
TOOLFUNC__ int16_t toolCoordGetY16(int32_t packed) { return (int16_t)(((uint32_t)packed >> 16) & 0xFFFF); }
TOOLFUNC__ int32_t toolCoordPack16(int16_t x, int16_t y) { return(int32_t)(((uint32_t)(uint16_t)x) | ((uint32_t)(uint16_t)y << 16)); }
TOOLFUNC__ int32_t toolCoordGetX32(int64_t packed) { return (int32_t)(packed & 0xFFFFFFFF); }
TOOLFUNC__ int32_t toolCoordGetY32(int64_t packed) { return (int32_t)(((uint64_t)packed >> 32) & 0xFFFFFFFF); }
TOOLFUNC__ int64_t toolCoordPack32(int32_t x, int32_t y) { return(int64_t)(((uint64_t)(uint32_t)x) | ((uint64_t)(uint32_t)y << 32)); }

TOOLFUNC__ intmax_t toolFloorDivideImpl(intmax_t a, intmax_t b) {
    TOOL_ASSERT(b != 0, "floorDiv: 除数不能为 0！");
    intmax_t q = a / b;
    intmax_t r = a % b;
    if (r != 0 && ((r ^ b) < 0)) {
        q--;
    }
    return q;
}

#ifndef TOOL_MATH_NO_SIMPLE_FUNC_NAME
#define average(...) toolAverageImpl((const double[]){__VA_ARGS__}, sizeof((const double[]){__VA_ARGS__}) / sizeof(double))
#define floorDiv(a, b) toolFloorDivideImpl((intmax_t)(a), (intmax_t)(b))
#define sum(...) toolSumImpl((const double[]){__VA_ARGS__}, sizeof((const double[]){__VA_ARGS__}) / sizeof(double))
#else
#define TOOL_AVERAGE(...) toolAverageImpl((const double[]){__VA_ARGS__}, sizeof((const double[]){__VA_ARGS__}) / sizeof(double))
#define TOOL_FLOOR_DIVIDE(a, b) toolFloorDivideImpl((intmax_t)(a), (intmax_t)(b))
#define TOOL_SUM(...) toolSumImpl((const double[]){__VA_ARGS__}, sizeof((const double[]){__VA_ARGS__}) / sizeof(double))
#endif // TOOL_MATH_NO_SIMPLE_FUNC_NAME

#endif // MMYMYS_TOOLS_MATH_INIT_H