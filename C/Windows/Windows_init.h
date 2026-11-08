// Windows/Windows_init.h

#pragma once

#ifdef UseWinTools

#ifndef _WIN32
#error 你不能在非 Windows 平台上使用这个文件，请删除 UseWinTools 的定义
#endif // _WIN32

#ifndef MMYMYS_TOOLS_WINDOWS_INIT_H
#define MMYMYS_TOOLS_WINDOWS_INIT_H

#include "WindowsBase/ToolWindowsBase.h"

#endif // MMYMYS_TOOLS_WINDOWS_INIT_H
#endif // UseWinTools