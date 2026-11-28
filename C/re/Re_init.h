// re/Re_init.h

#pragma once

#ifndef MMYMYS_TOOLS_RE_INIT_H
#define MMYMYS_TOOLS_RE_INIT_H

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../toolsfamily.h"
#include "../Vector/GeneralVector/ToolGeneralVector.h"

#define TOOL_RE_ARENA_SIZE 10240

#ifdef _MSC_VER
#define TOOL_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define TOOL_THREAD_LOCAL __thread
#else
#define TOOL_THREAD_LOCAL _Thread_local
#endif // _MSC_VER

#define toolrenulptr(T) (T*)NULL

typedef struct ToolReArena {
    char buffer[TOOL_RE_ARENA_SIZE];
    size_t offset;
}ToolReArena;

static TOOL_THREAD_LOCAL ToolReArena toolReArena = { .offset = 0 };

TOOLFUNC__ void toolReArenaReset() { toolReArena.offset = 0; }

TOOLFUNC__ void* toolReArenaAlloc(size_t size) {
    if (size > TOOL_RE_ARENA_SIZE) return NULL;
    size = (size + 7) & ~7;
    if (toolReArena.offset + size > TOOL_RE_ARENA_SIZE) toolReArenaReset();
    void* ptr = toolReArena.buffer + toolReArena.offset;
    toolReArena.offset += size;
    return ptr;
}

TOOLFUNC__ const char* toolReErrorFormat(const char* fmt, ...) {
    va_list arg, argCopy;
    va_start(arg, fmt);
    va_copy(argCopy, arg);
    intmax_t size = vsnprintf(NULL, 0, fmt, arg);
    if (size < 0) {
        va_end(arg);
        va_end(argCopy);
        return "Format error.";
    }
    size++;
    char* buffer = (char*)toolReArenaAlloc(size);
    if (!buffer) return "Format error: arena out of memory.";
    vsnprintf(buffer, size, fmt, argCopy);
    va_end(arg);
    va_end(argCopy);
    return buffer;
}

typedef struct ToolReCheckNamedGroupArr {
    uintmax_t key;
    const char* value;
} ToolReCheckNamedGroupArr;

typedef struct ToolReCheckResult {
    const char* msg;
    bool r;
} ToolReCheckResult;

typedef struct ToolReMatchResult {
    const char* msg;
    bool r;
} ToolReMatchResult;

#define TOOL_RE_CHECK_OK ((ToolReCheckResult){ .msg = NULL, .r = true })
#define TOOL_RE_CHECK_ERROR(errorMsg) \
    do {\
        errorMessage = errorMsg;\
        goto cleanup;\
    }\
    while(0)
#define TOOL_RE_CHECK_ERROR_FORMAT(...) TOOL_RE_CHECK_ERROR(toolReErrorFormat(__VA_ARGS__))
#define TOOL_RE_MATCH_OK ((ToolReMatchResult) { .msg = NULL, .r = true })
#define TOOL_RE_MATCH_ERROR(errorMsg) ((ToolReMatchResult) { .msg = errorMsg, .r = false })
#define TOOL_RE_MATCH_ERROR_FORMAT(...) TOOL_RE_MATCH_ERROR(toolReErrorFormat(__VA_ARGS__))

TOOL_CHKRET TOOLFUNC__ ToolReCheckResult toolReCheck(const char* reString) {
    TOOL_ASSERT(reString,"你这传的啥啊，让不让干活了？>_<");
    if (*reString == '\0') return TOOL_RE_CHECK_OK;
    TOOL_GEN_VECTOR_STRUCT_NAME namedGroupArr;
    TOOL_GEN_VECTOR_INIT(&namedGroupArr, sizeof(ToolReCheckNamedGroupArr));
    char* buffer = NULL;
    uintmax_t deep = 0;
    const char* errorMessage = NULL;
    uintmax_t groupIndexCount = 0;
    bool inStart = true;
    bool inInterval = false; // 用于 ^ 与 -
    bool inIntervalStart = false; // 用于 inIntervalSecond
    bool inIntervalSecond = false; // 用于 ^
    bool lastIsAtom = false; // 前面是不是原子
    bool lastIsQuantifier = false; // 前面是不是量词
    const char* start = reString;
    while (*start) {
        switch (*start) {
        case '$':
        {
            lastIsAtom = false;
            lastIsQuantifier = false;
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            const char next = *(start + 1);
            if (!(next == '\0' || next == ')' || next == '|')) TOOL_RE_CHECK_ERROR_FORMAT("$: 期望下一个为 \\0, ), |，得到%c", next);
            break;
        }
        case '*':
        case '+':
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            if (inStart) TOOL_RE_CHECK_ERROR_FORMAT("%c: 量词 %c 不能出现在开头", *start, *start);
            if (lastIsAtom) {
                lastIsAtom = false;
                lastIsQuantifier = true;
                break;
            }
            if (*start == '+') {
                if (lastIsQuantifier) {
                    lastIsAtom = false;
                    lastIsQuantifier = false;
                    break;
                }
            }
            TOOL_RE_CHECK_ERROR_FORMAT("%c: 量词 %c 不能出现在量词前", *start, *start);
        case '-':
            if (inInterval) {
                const char previous = *(start - 1);
                const char next = *(start + 1);
                lastIsAtom = false;
                lastIsQuantifier = false;
                if (previous == '[' || next == ']') break;
                if (previous > next) TOOL_RE_CHECK_ERROR_FORMAT("-: 区间 %c-%c 应为 %c-%c", previous, next, next, previous);
            }
            else {
                lastIsAtom = true;
                lastIsQuantifier = false;
            }
            break;
        case '?':
            if (inInterval) {
                lastIsAtom = false;
                lastIsQuantifier = false;
                break;
            }
            if (inStart) TOOL_RE_CHECK_ERROR_FORMAT("?: 量词 ? 不能出现在开头");
            if (lastIsAtom) {
                lastIsAtom = false;
                lastIsQuantifier = true;
            }
            else if (lastIsQuantifier) {
                lastIsAtom = false;
                lastIsQuantifier = false;
            }
            else TOOL_RE_CHECK_ERROR_FORMAT("?: 量词 ? 不能出现在%s %c 的后面", *(start - 1) == '?' ? "懒惰修饰词" : "其他不可被 ? 修饰的词", *(start - 1));
            break;
        case '[':
            if (inInterval) TOOL_RE_CHECK_ERROR_FORMAT("[: 禁止区间嵌套，如要匹配，请用 \"\\[\"");
            inInterval = true;
            inIntervalStart = true;
            break;
        case '\\':
            start++;
            switch (*start) {
            case '"':
            case '$':
            case '\'':
            case '(':
            case ')':
            case '*':
            case '+':
            case '-':
            case '.':
            case '?':
            case 'B':
            case 'D':
            case 'S':
            case 'W':
            case '[':
            case '\\':
            case ']':
            case '^':
            case 'b':
            case 'd':
            case 'f':
            case 'n':
                break;
            case 'o':
            {
                start++;
                if (*start != '{') TOOL_RE_CHECK_ERROR_FORMAT("\\o: 期望下一位为 {，得到 %c", *start);
                start++;
                int c = 0;
                while (*start >= '0' && *start <= '7') {
                    start++;
                    c++;
                    if (c > 6) TOOL_RE_CHECK_ERROR_FORMAT("\\o: 八进制数位过多");
                }
                if (!c) TOOL_RE_CHECK_ERROR_FORMAT("\\o: { 内部期望八进制数，但什么也没有得到，可能是您输入了非八进制数");
                if (*start != '}') TOOL_RE_CHECK_ERROR_FORMAT("\\o: 没有用于闭合的 }，可能是您输入了非八进制数");
                break;
            }
            case 'r':
            case 't':
            case 's':
            case 'v':
            case 'w':
                break;
            case 'x':
                for (int i = 0; i < 2; i++) {
                    if (*(start + 1) && isxdigit((unsigned char)*(start + 1))) {
                        start++;
                    }
                    else {
                        if (!i) TOOL_RE_CHECK_ERROR_FORMAT("\\x: 期望至少 1 位十六进制数，但什么也没得到");
                        break;
                    }
                }
                break;
            case '{':
            case '|':
            case '}':
                break;
            case 'u':
                for (int i = 0;i < 4;i++) {
                    if (!*(++start)) TOOL_RE_CHECK_ERROR_FORMAT("\\u: Unicode 转义期望 4 位十六进制数，得到 %" PRId32 " 位十六进制数", i);
                    if (!isxdigit((unsigned char)*start)) TOOL_RE_CHECK_ERROR_FORMAT("\\u: Unicode 转义期望十六进制数，得到 %c", *start);
                }
                break;
            case '0':
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                uint64_t group = *start - '0';
                start++;
                while (isdigit((unsigned char)*start)) {
                    group *= 10;group += *start - '0';
                    start++;
                }
                if (group > groupIndexCount) TOOL_RE_CHECK_ERROR_FORMAT("\\: %" PRId64 " 超出了反向引用分组的上限 %" PRIu64
                    " 如要使用 8 进制转义，请使用\"\\o{...}\"", group, groupIndexCount);
                start--;
                break;
            }
            default:
                TOOL_RE_CHECK_ERROR_FORMAT("\\: 未定义的转义符 %c", *start);
            }
            lastIsAtom = true;
            lastIsQuantifier = false;
            break;
        case '(':
        {

            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            deep++;
            lastIsAtom = false;
            lastIsQuantifier = false;
            start++;
            if (!*start) {
                start--;
                break; // 未闭合的括号，归后面管
            }
            if (*start == '?') {
                start++;
                switch (*start) {
                case '\0':
                    TOOL_RE_CHECK_ERROR_FORMAT("(: (? 后面缺少扩展语法的修饰符");
                case '!':
                case ':':
                    break;
                case '<':
                    start++;
                    switch (*start) {
                    case '!':
                    case '=':
                        break;
                    default:
                        TOOL_RE_CHECK_ERROR_FORMAT("(: ?< 后面只能是 !, =，不能是 %c", *start);
                    }
                    break;
                case '=':
                    break;
                case 'P':
                    start++;
                    if (*start == '<') {
                        start++;
                        size_t len = 0;
                        while (*start && (*start != '>')) {
                            if (!isalnum((unsigned char)*start) && *start != '_') TOOL_RE_CHECK_ERROR_FORMAT("(: 命名不合法");
                            if (!len && isdigit((unsigned char)*start)) TOOL_RE_CHECK_ERROR_FORMAT("(: 命名不合法");
                            len++;
                            start++;
                        }
                        if (!len) TOOL_RE_CHECK_ERROR_FORMAT("(: ?P 后面的标识名不能为空");
                        if (*start != '>') TOOL_RE_CHECK_ERROR_FORMAT("(: ?P< 未闭合，缺少 >");
                        start -= len;
                        buffer = (char*)calloc(len + 1, sizeof(char));
                        for (int i = 0;i < len;i++) {
                            buffer[i] = *start;
                            start++;
                        }
                        bool result = false;
                        for (size_t i = 0;i < namedGroupArr.len;i++) {
                            ToolReCheckNamedGroupArr* ptr = (ToolReCheckNamedGroupArr*)TOOL_GEN_VECTOR_GET(&namedGroupArr, i);
                            result = strcmp(buffer, ptr->value) == 0 ? true : false;
                            if (result) break;
                        }
                        if (result) TOOL_RE_CHECK_ERROR_FORMAT("(: ?P 重复定义分组 %s", buffer);
                        ToolReCheckNamedGroupArr newGroup = { .key = groupIndexCount + 1,.value = buffer };
                        TOOL_GEN_VECTOR_PUSH(&namedGroupArr, &newGroup);
                        buffer = NULL;
                        groupIndexCount++;
                        lastIsAtom = true;
                        lastIsQuantifier = false;
                        break;
                    }
                    else if (*start == '=') {
                        start++;
                        size_t len = 0;
                        while (*start && (*start != ')')) {
                            if (!isalnum((unsigned char)*start) && *start != '_') TOOL_RE_CHECK_ERROR_FORMAT("(: 命名不合法");
                            if (!len && isdigit((unsigned char)*start)) TOOL_RE_CHECK_ERROR_FORMAT("(: 命名不合法");
                            len++;
                            start++;
                        }
                        if (!len) TOOL_RE_CHECK_ERROR_FORMAT("(: ?P= 后面的标识名不能为空");
                        if (*start != ')') TOOL_RE_CHECK_ERROR_FORMAT("(: ?P= 未闭合，缺少 )");
                        start -= len;
                        buffer = (char*)calloc(len + 1, sizeof(char));
                        for (size_t i = 0;i < len;i++) {
                            buffer[i] = *start;
                            start++;
                        }
                        bool result = false;
                        for (size_t i = 0;i < namedGroupArr.len;i++) {
                            ToolReCheckNamedGroupArr* ptr = (ToolReCheckNamedGroupArr*)TOOL_GEN_VECTOR_GET(&namedGroupArr, i);
                            result = strcmp(buffer, ptr->value) == 0 ? true : false;
                            if (result) break;
                        }
                        if (result) {
                            free(buffer);
                            buffer = NULL;
                            start--;
                            lastIsAtom = true;
                            lastIsQuantifier = false;
                            break;
                        }
                        TOOL_RE_CHECK_ERROR_FORMAT("(: ?P= 的命名错误，未找到分组 %s", buffer);
                    }
                    else TOOL_RE_CHECK_ERROR_FORMAT("(: ?P  后面期望 <, =，得到 %c", *start);
                default:
                    TOOL_RE_CHECK_ERROR_FORMAT("(: (? 后面缺少扩展语法的修饰符");
                }
            }
            else groupIndexCount++;
            break;
        }
        case ')':
            lastIsAtom = true;
            lastIsQuantifier = false;
            if ((deep > 0) && (!inInterval)) deep--;
            else TOOL_RE_CHECK_ERROR_FORMAT("): 括号递归深度为 0！若要匹配，请用 \"\\)\"");
            break;
        case ']':
            if ((!inInterval) || inIntervalSecond) TOOL_RE_CHECK_ERROR_FORMAT("]: %s", 
                inInterval ? "区间不能为空，如要匹配，请用 \"\\[\\]\"" : "] 不能出现在非区间地，如要匹配，请用 \"\\]\"");
            inInterval = false;
            lastIsAtom = true;
            lastIsQuantifier = false;
            break;
        case '^':
            if (inStart || (inInterval && inIntervalSecond)) {
                lastIsAtom = false;
                lastIsQuantifier = false;
                break;
            }
            TOOL_RE_CHECK_ERROR_FORMAT("^: ^ 不能出现在%s！如要匹配，请用 \"\\^\"", inInterval ? "非区间开头" : "非开头");
        case '{':
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            if (inStart || !lastIsAtom) TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 { 不能出现在非原子前");
            {
                uint64_t a = (uint64_t)-1, b = (uint64_t)-1;
                start++;
                if (*start >= '0' && *start <= '9') a = 0;
                else TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 { 后面期望数字，得到 %c", *start);
                while ((*start >= '0' && *start <= '9')) {
                    a *= 10;
                    a += *start - '0';
                    start++;
                }
                if (*start != '\0' && (*start == '}' || *(start + 1) == '}')) {
                    start -= (*start == '}') ? 1 : 0;
                    break;
                }
                else if (*start == ',') {
                    start++;
                    if (*start >= '0' && *start <= '9') b = 0;
                    else TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 ", 后面期望数字，得到 %c", a, *start);
                    while ((*start >= '0' && *start <= '9')) {
                        b *= 10;
                        b += *start - '0';
                        start++;
                    }
                    if (*start == '}') {
                        if (a <= b) {
                            start--;
                            break;
                        }
                        else TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 ",%" PRIu64 "} 范围错误", a, b);
                    }
                    else TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 ",%" PRIu64 " 期望 }，得到 %c", a, b, *start);
                }
                else TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 " 期望 }, ,，得到 %c", a, *start);
            }
            break;
        case '|':
        {
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            if (inStart || *(start + 1) == '\0') TOOL_RE_CHECK_ERROR_FORMAT("|: 或运算符 | 不能出现在%s", inStart ? "开头" : "结尾");
            const char previous = *(start - 1);
            const char next = *(start + 1);
            if (!(lastIsAtom || lastIsQuantifier || previous == ')')) TOOL_RE_CHECK_ERROR_FORMAT("|: 或运算符 | 期望上一个是原子, 量词, 分组，得到 %c", previous);
            bool nextIsValidBranchStart = (
                next == '$' ||
                next == '(' ||
                next == '.' ||
                (
                    next >= '0' &&
                    next <= '9'
                    ) ||
                (
                    next >= 'A' &&
                    next <= 'Z'
                    ) ||
                next == '[' ||
                next == '\\' ||
                next == '^' ||
                next == '_' ||
                (
                    next >= 'a' &&
                    next <= 'z'
                    )
                );
            if (!nextIsValidBranchStart) TOOL_RE_CHECK_ERROR_FORMAT("|: 或运算符 | 期望下一个是 原子, 合法表达式 得到 %c，"
                "如果您的原子 / 表达式开头不在三十六进制数, $, (, ., [, \\, ^, _ 内，请尝试使用 Unicode 转义", next);
            lastIsAtom = false;
            lastIsQuantifier = false;
            break;
        }
        case '}':
        {
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            if (inStart) TOOL_RE_CHECK_ERROR_FORMAT("}: } 不能在开头");
            const char previous = *(start - 1);
            if (!(previous == ',' || (previous >= '0' && previous <= '9'))) TOOL_RE_CHECK_ERROR_FORMAT("}: } 期望上一个为 ,, 0~9，得到%c", previous);
            lastIsAtom = false;
            lastIsQuantifier = true;
            break;
        }
        default:
            if (!inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
            }
        }
        if (inStart) inStart = false;
        if (inIntervalSecond) inIntervalSecond = false;
        if (inIntervalStart) inIntervalSecond = true;
        if (inIntervalStart) inIntervalStart = false;
        start++;
    }
    if (deep) TOOL_RE_CHECK_ERROR_FORMAT("(: 括号未闭合，深度 %" PRIu64 " 若要匹配，请用\"\\(\"", deep);
    if (inInterval) TOOL_RE_CHECK_ERROR_FORMAT("[: 区间未闭合");
cleanup:
    for (size_t i = 0; i < namedGroupArr.len; i++) {
        ToolReCheckNamedGroupArr* ptr = (ToolReCheckNamedGroupArr*)TOOL_GEN_VECTOR_GET(&namedGroupArr, i);
        if (ptr && ptr->value) free((void*)ptr->value);
    }
    TOOL_GEN_VECTOR_FREE_IMPL(&namedGroupArr, true);
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
    if (errorMessage) return ((ToolReCheckResult) { .msg = errorMessage, .r = false });
    return TOOL_RE_CHECK_OK;
}

TOOLFUNC__ ToolReMatchResult toolReMatch(const char* reString, const char* text) {
    TOOL_ASSERT(reString, "你这传的啥啊，让不让干活了？>_<");
    ToolReCheckResult r = toolReCheck(reString);
    if (!(r.r)) return TOOL_RE_MATCH_ERROR(r.msg); // toolReCheck 返回的已经是格式化字符串了，无需再调用 toolReErrorFormat
    // TODO 补全
    return TOOL_RE_MATCH_OK;
}

#endif // MMYMYS_TOOLS_RE_INIT_H