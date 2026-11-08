// String/String_init.h

#pragma once

#ifndef MMYMYS_TOOLS_STRING_INIT_H
#define MMYMYS_TOOLS_STRING_INIT_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../toolsfamily.h"

TOOL_CHKRET TOOLFUNC__ bool toolStringIEquals(const char* s1, const char* s2) {
    if (s1 == s2) return true;
    if (s1 == NULL || s2 == NULL) return false;
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))return false;
        s1++;s2++;
    }
    return *s1 == *s2;
}

TOOLFUNC__ char* toolStringTrim(char* str) {
    if (!str) return NULL;
    char* start = str;
    while (*start && isspace((unsigned char)*start))start++;
    if (*start == '\0') {
        *str = '\0';
        return str;
    }
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end))end--;
    *(end + 1) = '\0';
    return start;
}

TOOL_CHKRET TOOLFUNC__ bool toolStringStartMatch(const char* const str, const char* const prefix) {
    if (!str || !prefix) return false;
    size_t prefixLen = strlen(prefix);
    if (prefixLen > strlen(str)) return false;
    return strncmp(str, prefix, prefixLen) == 0;
}

TOOL_CHKRET TOOLFUNC__ bool toolStringEndMatch(const char* const str, const char* const suffix) {
    if (!str || !suffix) return false;
    size_t strLen = strlen(str);
    size_t suffixLen = strlen(suffix);
    if (suffixLen > strLen) return false;
    return strcmp(str + strLen - suffixLen, suffix) == 0;
}

TOOLFUNC__ char* toolStringToLower(char* const str) {
    if (!str) return NULL;
    char* p = str;
    while (*p) {
        *p = (char)tolower((unsigned char)*p);
        p++;
    }
    return str;
}

TOOLFUNC__ char* toolStringToUpper(char* const str) {
    if (!str) return NULL;
    char* p = str;
    while (*p) {
        *p = (char)toupper((unsigned char)*p);
        p++;
    }
    return str;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 6001)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
#endif // _MSC_VER
TOOLFUNC__ void toolStringSplitFree(char** result) {
    if (!result) return;
    for (size_t i = 0; result[i] != NULL; i++) {
        free(result[i]);
    }
    free(result);
}

TOOLFUNC__ char** toolStringSplit(const char* str, const char* delimiter) {
    if (!str || !delimiter || *delimiter == '\0') return NULL;
    size_t count = 1;
    const char* p = str;
    size_t delimLen = strlen(delimiter);
    while ((p = strstr(p, delimiter)) != NULL) {
        count++;
        p += delimLen;
    }
    char** result = (char**)calloc((count + 1), sizeof(char*));
    if (!result) return NULL;
    size_t index = 0;
    const char* start = str;
    p = str;
    while ((p = strstr(p, delimiter)) != NULL) {
        size_t len = p - start;
        result[index] = (char*)malloc(len + 1);
        if (!result[index]) {
            toolStringSplitFree(result);
            return NULL;
        }
        memcpy(result[index], start, len);
        result[index][len] = '\0';
        index++;
        p += delimLen;
        start = p;
    }
    size_t lastLen = strlen(start);
    result[index] = (char*)malloc(lastLen + 1);
    if (!result[index]) {
        toolStringSplitFree(result);
        return NULL;
    }
    memcpy(result[index], start, lastLen);
    result[index][lastLen] = '\0';
    result[count] = NULL;
    return result;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // MMYMYS_TOOLS_STRING_INIT_H