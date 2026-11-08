// Vector/GeneralVector/ToolGeneralVector.h

#pragma once

#ifndef MMYMYS_TOOLS_GENERAL_VECTOR_H
#define MMYMYS_TOOLS_GENERAL_VECTOR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../toolsfamily.h"

#define TOOL_EXPAND_GV(Parameter) Parameter
#define TOOL_GET_NAME_3_GV(_1, _2, _3, ...) _3
#define TOOL_GEN_VECTOR_FREE_1(vec, bol) TOOL_GEN_VECTOR_FREE_IMPL(vec, bol)
#define TOOL_GEN_VECTOR_FREE_2(vec) TOOL_GEN_VECTOR_FREE_IMPL(vec, false)

#ifndef TOOL_GENERAL_VECTOR_NO_SIMPLE_STRUCT_NAME
#define TOOL_GEN_VECTOR_STRUCT_NAME GenVector
#else
#define TOOL_GEN_VECTOR_STRUCT_NAME TOOL_GenVector
#endif // TOOL_GENERAL_VECTOR_NO_SIMPLE_STRUCT_NAME

#ifndef TOOL_GENERAL_VECTOR_NO_SIMPLE_FUNC_NAME
#define TOOL_GEN_VECTOR_INIT genVectorInit
#define TOOL_GEN_VECTOR_PUSH genVectorPush
#define TOOL_GEN_VECTOR_POP genVectorPop
#define TOOL_GEN_VECTOR_CLEAR genVectorClear
#define TOOL_GEN_VECTOR_FREE_IMPL genVectorFreeImpl
#define genVectorFree(...) TOOL_EXPAND_GV(TOOL_GET_NAME_3_GV(__VA_ARGS__, TOOL_GEN_VECTOR_FREE_1, TOOL_GEN_VECTOR_FREE_2))(__VA_ARGS__)
#else
#define TOOL_GEN_VECTOR_INIT toolGeneralVectorInit
#define TOOL_GEN_VECTOR_PUSH toolGeneralVectorPushBack
#define TOOL_GEN_VECTOR_POP toolGeneralVectorPopBack
#define TOOL_GEN_VECTOR_CLEAR toolGeneralVectorClear
#define TOOL_GEN_VECTOR_FREE_IMPL toolGeneralVectorFreeImpl
#define toolGeneralVectorFree(...) TOOL_EXPAND_GV(TOOL_GET_NAME_3_GV(__VA_ARGS__, TOOL_GEN_VECTOR_FREE_1, TOOL_GEN_VECTOR_FREE_2))(__VA_ARGS__)
#endif // TOOL_GENERAL_VECTOR_NO_SIMPLE_FUNC_NAME

typedef struct {
	void* start;
	size_t len;
	size_t max;
	size_t size;
} TOOL_GEN_VECTOR_STRUCT_NAME;

TOOLFUNC__ void TOOL_GEN_VECTOR_INIT(TOOL_GEN_VECTOR_STRUCT_NAME* const vec, size_t size) {
    TOOL_ASSERT(vec != NULL, "Vector 不能为空");
	vec->start = NULL;
	vec->len = 0;
	vec->max = 0;
	vec->size = size;
}

TOOL_CHKRET TOOLFUNC__ bool TOOL_GEN_VECTOR_PUSH(TOOL_GEN_VECTOR_STRUCT_NAME* const vec, const void* const p) {
	TOOL_ASSERT(vec != NULL, "Vector 不能为空");
	TOOL_ASSERT(p != NULL, "要添加的数据不能为空");
	if (vec->len >= vec->max) {
		size_t newSize = vec->max ? (vec->max + (vec->max >> 1)) : 4;
#ifdef RSIZE_MAX
		if (newSize > RSIZE_MAX / vec->size)
#elif defined(SIZE_MAX)
        if (newSize > SIZE_MAX / vec->size)
#else
        if (newSize > (size_t)-1 / vec->size)
#endif // RSIZE_MAX
            return false;
		void* newMemory = realloc(vec->start, newSize * vec->size);
		if (!newMemory)return false;
		vec->start = newMemory;
		vec->max = newSize;
	}
	char* target = (char*)vec->start + (vec->len * vec->size);
	memcpy(target, p, vec->size);
	vec->len++;
	return true;
}

TOOL_CHKRET TOOLFUNC__ bool TOOL_GEN_VECTOR_POP(TOOL_GEN_VECTOR_STRUCT_NAME* const vec, void* const out) {
    TOOL_ASSERT(vec != NULL, "Vector 不能为空");
    if (vec->len == 0) return false;
    vec->len--;
    if (out != NULL) {
        const char* source = (const char*)vec->start + (vec->len * vec->size);
        memcpy(out, source, vec->size);
    }
    if (vec->max > 4 && vec->len <= vec->max / 4) {
        size_t newMax = ((vec->max >> 2) < 4) ? 4 : (vec->max >> 2);
        void* newMemory = realloc(vec->start, newMax * vec->size);
        if (newMemory) {
            vec->start = newMemory;
            vec->max = newMax;
        }
    }
    return true;
}

TOOLFUNC__ void TOOL_GEN_VECTOR_FREE_IMPL(TOOL_GEN_VECTOR_STRUCT_NAME* const vec, bool bol) {
    if (vec == NULL) return;
    if (vec->start != NULL) {
        free(vec->start);
        vec->start = NULL;
    }
    vec->len = 0;
    vec->max = 0;
    if (bol) {
        vec->size = 0;
    }
}

TOOLFUNC__ void TOOL_GEN_VECTOR_CLEAR(TOOL_GEN_VECTOR_STRUCT_NAME* const vec) {
    if (vec == NULL) return;
    vec->len = 0;
}

#endif // MMYMYS_TOOLS_GENERAL_VECTOR_H