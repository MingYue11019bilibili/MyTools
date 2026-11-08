// Windows/WindowsBase/ToolWindowsBase.h

#pragma once

#ifndef MMYMYS_TOOLS_WINDOWS_BASE_H
#define MMYMYS_TOOLS_WINDOWS_BASE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <Windows.h>

#include "../../toolsfamily.h"
#include "ToolWindowsBaseConsole.h"

typedef struct {
	LARGE_INTEGER startCount;
	LARGE_INTEGER frequency;
}ToolWinTimer;

TOOL_CHKRET TOOLFUNC__ bool toolWinPrintWchar(const wchar_t* const str) {
    if (!str) return false;
    size_t len = wcslen(str);
    if (!len)return true;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE || hConsole == NULL) return false;
    DWORD fileType = GetFileType(hConsole);
    if (fileType == FILE_TYPE_DISK || fileType == FILE_TYPE_PIPE) {
        bool rt = (fwprintf(stdout, L"%ls", str) >= 0);
        fflush(stdout);
        return rt;
    }
    DWORD written = 0;
    return WriteConsoleW(hConsole, str, (DWORD)len, &written, NULL) != 0;
}

TOOL_CHKRET TOOLFUNC__ bool toolWinPrintWcharLn(const wchar_t* const str) {
    return toolWinPrintWchar(str) && toolWinPrintWchar(L"\n");
}

TOOLFUNC__ double toolWinTimerGetSeconds(const ToolWinTimer* const timer) {
    if (!timer || timer->frequency.QuadPart == 0) return 0.0;
    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&currentCount);
    return (double)(currentCount.QuadPart - timer->startCount.QuadPart) / (double)timer->frequency.QuadPart;
}

TOOLFUNC__ uint64_t toolWinTimerGetMillis(const ToolWinTimer* const timer) {
    double sec = toolWinTimerGetSeconds(timer);
    return (uint64_t)(sec * 1000.0);
}

TOOLFUNC__ void toolWinTimerStart(ToolWinTimer* const timer) {
	TOOL_ASSERT(timer, "toolWinTimerStart:timer 不能为空");
	QueryPerformanceFrequency(&timer->frequency);
	QueryPerformanceCounter(&timer->startCount);
}

TOOLFUNC__ wchar_t* toolWinUtf8ToWchar(const char* const utf8Str) {
    if (!utf8Str) return NULL;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    if (wlen <= 0) return NULL;
    wchar_t* wcharStr = (wchar_t*)malloc(wlen * sizeof(wchar_t));
    if (!wcharStr)return NULL;
    MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wcharStr, wlen);
    return wcharStr;
}

TOOLFUNC__ char* toolWinWcharToUtf8(const wchar_t* const wcharStr) {
    if (!wcharStr) return NULL;
    int len = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, NULL, 0, NULL, NULL);
    if (len <= 0) return NULL;
    char* utf8Str = (char*)malloc(len);
    if (!utf8Str)return NULL;
    WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, utf8Str, len, NULL, NULL);
    return utf8Str;
}

#endif // MMYMYS_TOOLS_WINDOWS_BASE_H