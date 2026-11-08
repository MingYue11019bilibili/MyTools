// Vector/TemplateVector/ToolTemplateVector.h

#pragma once

#ifndef MMYMYS_TOOLS_TEMPLATE_VECTOR_H
#define MMYMYS_TOOLS_TEMPLATE_VECTOR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../toolsfamily.h"

#ifdef RSIZE_MAX
#define TOOL_SIZE_EQ_IPML RSIZE_MAX
#elif defined(SIZE_MAX)
#define TOOL_SIZE_EQ_IPML SIZE_MAX
#else
#define TOOL_SIZE_EQ_IPML (size_t)-1
#endif // RSIZE_MAX

#define TOOL_SIZE_EQ TOOL_SIZE_EQ_IPML

#define TOOL_DEFINE_VECTOR(T, NAME) \
    \
    typedef struct { \
        T* start; \
        size_t len; \
        size_t max; \
    } TOOL_TemVector_##T##_##NAME; \
    \
    TOOLFUNC__ void toolTemplateVectorInit_##T##_##NAME(TOOL_TemVector_##T##_##NAME* const vec) { \
        TOOL_ASSERT(vec != NULL, "Vector 不能为空"); \
        vec->start = NULL; \
        vec->len = 0; \
        vec->max = 0; \
    } \
    \
    TOOL_CHKRET TOOLFUNC__ bool toolTemplateVectorPushBack_##T##_##NAME(TOOL_TemVector_##T##_##NAME* const vec, T p) { \
        TOOL_ASSERT(vec != NULL, "Vector 不能为空"); \
        if (vec->len >= vec->max) { \
            size_t newSize = vec->max ? (vec->max + (vec->max >> 1)) : 4; \
            if (newSize > TOOL_SIZE_EQ / sizeof(T)) return false; \
            T* newMemory = (T*)realloc(vec->start, newSize * sizeof(T)); \
            if (!newMemory) return false; \
            vec->start = newMemory; \
            vec->max = newSize; \
        } \
        vec->start[vec->len++] = p; \
        return true; \
    } \
    \
    TOOL_CHKRET TOOLFUNC__ bool toolTemplateVectorPopBack_##T##_##NAME(TOOL_TemVector_##T##_##NAME* const vec, T* const out) { \
        TOOL_ASSERT(vec != NULL, "Vector 不能为空"); \
        if (vec->len == 0) return false; \
        vec->len--; \
        if (out != NULL) { \
            *out = vec->start[vec->len]; \
        } \
        if (vec->max > 4 && vec->len <= (vec->max >> 2)) { \
            size_t newMax = ((vec->max >> 1) < 4) ? 4 : (vec->max >> 1); \
            T* newMemory = (T*)realloc(vec->start, newMax * sizeof(T)); \
            if (newMemory) { \
                vec->start = newMemory; \
                vec->max = newMax; \
            } \
        } \
        return true; \
    } \
    \
    TOOLFUNC__ void toolTemplateVectorFree_##T##_##NAME(TOOL_TemVector_##T##_##NAME* const vec) { \
        if (vec == NULL) return; \
        if (vec->start != NULL) { \
            free(vec->start); \
            vec->start = NULL; \
        } \
        vec->len = 0; \
        vec->max = 0; \
    } \
    \
    TOOLFUNC__ void toolTemplateVectorClear_##T##_##NAME(TOOL_TemVector_##T##_##NAME* const vec) { \
        if (vec == NULL) return; \
        vec->len = 0; \
    }

#endif // MMYMYS_TOOLS_TEMPLATE_VECTOR_H