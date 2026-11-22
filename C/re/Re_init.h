// re/Re_init.h

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../toolsfamily.h"

#ifdef _MSC_VER
#define TOOL_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define TOOL_THREAD_LOCAL __thread
#else
#define TOOL_THREAD_LOCAL
#endif // _MSC_VER

TOOLFUNC__ const char* toolReCheckErrorFormat(const char* fmt, ...) {
    static TOOL_THREAD_LOCAL char buf[4096]; // 4 KiB 不算多，但也几乎能存储所有数据了
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return buf;
}

typedef struct {
    bool r;
    const char* msg;
} ToolReCheckResult;

#define TOOL_RE_CHECK_OK ((ToolReCheckResult){ .r = true, .msg = NULL })
#define TOOL_RE_CHECK_ERROR(errorMsg) ((ToolReCheckResult){ .r = false, .msg = errorMsg })
#define TOOL_RE_CHECK_ERROR_FORMAT(...) TOOL_RE_CHECK_ERROR(toolReCheckErrorFormat(__VA_ARGS__))

TOOLFUNC__ ToolReCheckResult toolReCheck(const char* reString) {
    TOOL_ASSERT(reString,"你这传的啥啊，让不让干活了？>_<");
    if (*reString == '\0') return TOOL_RE_CHECK_OK;
    const char* start = reString;
    uintmax_t deep = 0;
    bool inStart = true;
    bool inInterval = false; // 用于 ^ 与 -
    bool inIntervalStart = false; // 用于 inIntervalSecond
    bool inIntervalSecond = false; // 用于 ^
    bool lastIsAtom = false; // 前面是不是原子
    bool lastIsQuantifier = false; // 前面是不是量词
    while (*start) {
        switch (*start) {
        case '$':
        {
            lastIsAtom = false;
            lastIsQuantifier = false;
            if (inInterval) {
                lastIsAtom = true;
                break;
            }
            const char next = *(start + 1);
            if (!(next == '\0' || next == ')' || next == '|')) return TOOL_RE_CHECK_ERROR_FORMAT("$: 期望下一个为 \\0, ), |，得到%c", next);
            break;
        }
        case '*':
        case '+':
            if (inInterval) {
                lastIsAtom = false;
                lastIsQuantifier = false;
                break;
            }
            if (inStart) return TOOL_RE_CHECK_ERROR_FORMAT("%c: 量词 %c 不能出现在开头", *start, *start);
            if (lastIsAtom) {
                lastIsAtom = false;
                lastIsQuantifier = true;
                break;
            }
            return TOOL_RE_CHECK_ERROR_FORMAT("%c: 量词 %c 不能出现在量词前", *start, *start);
        case '-':
            if (inInterval) {
                const char previous = *(start - 1);
                const char next = *(start + 1);
                lastIsAtom = false;
                lastIsQuantifier = false;
                if (previous == '[' || next == ']') break;
                if (previous > next) return TOOL_RE_CHECK_ERROR_FORMAT("-: 区间 %c-%c 应为 %c-%c", previous, next, next, previous);
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
            if (inStart) return TOOL_RE_CHECK_ERROR_FORMAT("?: 量词 ? 不能出现在开头");
            if (lastIsAtom) {
                lastIsAtom = false;
                lastIsQuantifier = true;
            }
            else if (lastIsQuantifier) {
                lastIsAtom = false;
                lastIsQuantifier = false;
            }
            else return TOOL_RE_CHECK_ERROR_FORMAT("?: 量词 ? 不能出现在%s %c 的后面", *(start - 1) == '?' ? "懒惰修饰词" : "其他不可被 ? 修饰的词", *(start - 1));
            break;
        case '[':
            if (inInterval) return TOOL_RE_CHECK_ERROR_FORMAT("[: 禁止区间嵌套，如要匹配，请用 \"\\[\"");
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
            case 'r':
            case 't':
            case 's':
            case 'v':
            case 'w':
            case '{':
            case '|':
            case '}':
                break;
            case 'u':
                for (int i = 0;i < 4;i++) {
                    if (!*(++start)) return TOOL_RE_CHECK_ERROR_FORMAT("\\u: Unicode 转义期望 4 位十六进制数，得到 %" PRId32 " 位十六进制数", i);
                    if (!isxdigit((unsigned char)*start)) return TOOL_RE_CHECK_ERROR_FORMAT("\\u: Unicode 转义期望十六进制数，得到 %c", *start);
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            {
                int c = 1;
                while (c < 3 && *(start + 1) >= '0' && *(start + 1) <= '7') {
                    start++;
                    c++; // C++？Wow 还能串！？
                }
                break;
            }
            default:
                return TOOL_RE_CHECK_ERROR_FORMAT("\\: 未定义的转义符 %c", *start);
            }
            lastIsAtom = true;
            lastIsQuantifier = false;
            break;
        case '(':
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            deep++;
            lastIsAtom = false;
            lastIsQuantifier = false;
            break;
        case ')':
            lastIsAtom = true;
            lastIsQuantifier = false;
            if ((deep > 0) && (!inInterval)) deep--;
            else return TOOL_RE_CHECK_ERROR_FORMAT("): 括号递归深度为 0！若要匹配，请用 \"\\)\"");
            break;
        case ']':
            if ((!inInterval) || inIntervalSecond) return TOOL_RE_CHECK_ERROR_FORMAT("]: %s", 
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
            return TOOL_RE_CHECK_ERROR_FORMAT("^: ^ 不能出现在%s！如要匹配，请用 \"\\^\"", inInterval ? "非区间开头" : "非开头");
        case '{':
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            if (inStart || !lastIsAtom) return TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 { 不能出现在非原子前");
            {
                uint64_t a = (uint64_t)-1, b = (uint64_t)-1;
                start++;
                if (*start >= '0' && *start <= '9') a = 0;
                else return TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 { 后面期望数字，得到 %c", *start);
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
                    else return TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 ", 后面期望数字，得到 %c", a, *start);
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
                        else return TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 ",%" PRIu64 "} 范围错误", a, b);
                    }
                    else return TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 ",%" PRIu64 " 期望 }，得到 %c", a, b, *start);
                }
                else return TOOL_RE_CHECK_ERROR_FORMAT("{: 量词 {%" PRIu64 " 期望 }, ,，得到 %c", a, *start);
            }
            break;
        case '|':
        {
            if (inInterval) {
                lastIsAtom = true;
                lastIsQuantifier = false;
                break;
            }
            if (inStart || *(start + 1) == '\0') return TOOL_RE_CHECK_ERROR_FORMAT("|: 或运算符 | 不能出现在%s", inStart ? "开头" : "结尾");
            const char previous = *(start - 1);
            const char next = *(start + 1);
            if (!(lastIsAtom || lastIsQuantifier || previous == ')')) return TOOL_RE_CHECK_ERROR_FORMAT("|: 或运算符 | 期望上一个是原子, 量词, 分组，得到 %c", previous);
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
            if (!nextIsValidBranchStart) return TOOL_RE_CHECK_ERROR_FORMAT("|: 或运算符 | 期望下一个是 原子, 合法表达式 得到 %c，"
                "如果您的原子 / 表达式开头 不在 三十六进制数, $, (, ., [, \\, ^, _ 内，请尝试使用 Unicode 转义", next);
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
            if (inStart) return TOOL_RE_CHECK_ERROR_FORMAT("}: } 不能在开头");
            const char previous = *(start - 1);
            if (!(previous == ',' || (previous >= '0' && previous <= '9'))) return TOOL_RE_CHECK_ERROR_FORMAT("}: } 期望上一个为 ,, 0~9，得到%c", previous);
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
    if (deep)return TOOL_RE_CHECK_ERROR_FORMAT("(: 括号未闭合，深度 %" PRIu64 " 若要匹配，请用\"\\(\"");
    if (inInterval) return TOOL_RE_CHECK_ERROR_FORMAT("[: 区间未闭合");
    return TOOL_RE_CHECK_OK;
}