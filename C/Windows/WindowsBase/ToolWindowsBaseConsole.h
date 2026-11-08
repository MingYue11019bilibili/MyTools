// Windows/WindowsBase/ToolWindowsBaseConsole.h

#pragma once

#ifndef MMYMYS_TOOLS_WINDOWS_BASE_CONSOLE_H
#define MMYMYS_TOOLS_WINDOWS_BASE_CONSOLE_H

#include <stdbool.h>
#include <stdio.h>
#include <Windows.h>

#include "../../toolsfamily.h"

typedef enum {
    TOOL_COLOR_BLACK = 0,
    TOOL_COLOR_BLUE = 1,
    TOOL_COLOR_GREEN = 2,
    TOOL_COLOR_CYAN = 3,
    TOOL_COLOR_RED = 4,
    TOOL_COLOR_MAGENTA = 5,
    TOOL_COLOR_YELLOW = 6,
    TOOL_COLOR_WHITE = 7,
    TOOL_COLOR_GRAY = 8,
    TOOL_COLOR_LIGHT_BLUE = 9,
    TOOL_COLOR_LIGHT_GREEN = 10,
    TOOL_COLOR_LIGHT_CYAN = 11,
    TOOL_COLOR_LIGHT_RED = 12,
    TOOL_COLOR_LIGHT_MAGENTA = 13,
    TOOL_COLOR_LIGHT_YELLOW = 14,
    TOOL_COLOR_BRIGHT_WHITE = 15
} ToolConsoleColor;

TOOLFUNC__ void toolConsoleClear(void) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = { 0, 0 };
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hConsole, homeCoords);
}

TOOLFUNC__ void toolConsoleSetColor(ToolConsoleColor textColor, ToolConsoleColor bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;
    SetConsoleTextAttribute(hConsole, (WORD)((bgColor << 4) | textColor));
}

TOOLFUNC__ void toolConsoleResetColor(void) {
    toolConsoleSetColor(TOOL_COLOR_WHITE, TOOL_COLOR_BLACK);
}

TOOLFUNC__ void toolConsoleSetCursor(short x, short y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;
    COORD coord = { x, y };
    SetConsoleCursorPosition(hConsole, coord);
}

TOOLFUNC__ void toolConsoleSetCursorVisible(bool visible) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

#endif // MMYMYS_TOOLS_WINDOWS_BASE_CONSOLE_H