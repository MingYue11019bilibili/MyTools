// toolsfamily.h

#pragma once

#ifndef MMYMYS_TOOLS_FAMILY_H
#define MMYMYS_TOOLS_FAMILY_H

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#define TOOL_SLEEP_IMPL(ms) Sleep(ms)
#else
#include <unistd.h>
#define TOOL_SLEEP_IMPL(ms) usleep((useconds_t)((unsigned long long)(ms) * 1000ULL))
#endif // _WIN32

#define TOOL_SLEEP(ms) TOOL_SLEEP_IMPL(ms)

// 定义强制检查返回值宏
#if defined(__cplusplus) && __cplusplus >= 201703L
#define TOOL_CHKRET [[nodiscard]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#define TOOL_CHKRET [[nodiscard]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define TOOL_CHKRET __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define TOOL_CHKRET _Check_return_
#else
#define TOOL_CHKRET
#endif // defined(__GNUC__) || defined(__clang__)
#endif // defined(__cplusplus) && __cplusplus >= 201703L

#define TOOL_LOG(...) printf(__VA_ARGS__)
#define TOOL_ERR(fmt, ...) fprintf(stderr, "\033[31m[ERROR] " fmt "\033[0m\n", ##__VA_ARGS__)
#define TOOL_WARN(fmt, ...) fprintf(stderr, "\033[33m[WARN] " fmt "\033[0m\n", ##__VA_ARGS__)
#define TOOLFUNC__ static inline
#define TOOL_ASSERT(cond, msg) do { if (!(cond)){ fflush(stdout);fprintf(stderr, "\033[31m[命中断言！]\033[0m %s\n文件: %s\n行: %d\n条件: %s\n", msg, __FILE__, __LINE__, #cond);abort(); } } while (0)

#endif // MMYMYS_TOOLS_FAMILY_H