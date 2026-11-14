# MyTools - 一个好用的 C 语言单头文件工具库

![C Standard](https://img.shields.io/badge/C-C99%2FC11-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![Made by](https://img.shields.io/badge/Made_by-MingYue11019bilibili-green?logo=github)
![Passed](https://img.shields.io/badge/Windows-MSVC_100%25_Passed-5C2D91?logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNTYgMjY0IiBwcmVzZXJ2ZUFzcGVjdFJhdGlvPSJ4TWlkWU1pZCI%2BDQogIDxnPg0KICAgIDxwYXRoIGQ9Ik0xODguMzcwMTA0LDI2My42NDY4NTMgTDkwLjQ0NjAyMzksMTY4LjYwNjU5OCBMMzIuNDk0MTUxNCwyMTIuNzIwNzAxIEwwLjUwODk0MDIzOSwxOTYuNjc1MzE1IEwwLjUzMzQxODMyNyw2Ny4yMTE3MjkxIEwzMi42MTgwNzE3LDUxLjA3NTU2OTcgTDkwLjYxNDMxMDgsOTUuMjIzODQwNiBMMTg4LjI5OTIxOSwwLjA5NDM0MjYyOTUgTDI1NS40Njc2MDIsMjcuMDE5MjE5MSBMMjU1LjQ5MDU1LDIzNi44MjQ0NzggTDE4OC4zNzAxMDQsMjYzLjY0Njg1MyIgZmlsbD0iI0ZGRkZGRiI%2BPC9wYXRoPg0KICAgIDxwYXRoIGQ9Ik0yMzguNDcxNjQ5LDM4LjUxODMxMDggTDE5Mi4yMzc2NDEsMTkuOTg0ODI4NyBMOTEuOTc1OTA0NCwxMTcuNjIyMzExIEwzMC43NzA0ODYxLDcxLjAzMDgyMDcgTDE3LjUyODg2MDYsNzcuNjkwMzkwNCBMMTcuNTA3OTUyMiwxODYuMTg3NDc0IEwzMC42MzY4NzY1LDE5Mi43NzMxIEw5MS43ODg3NDksMTQ2LjIyMjkxNiBMMTkyLjI5ODMyNywyNDMuNzczMTk1IEwyMzguNDkyNTU4LDIyNS4zMTM2NTcgTDIzOC40NzE2NDksMzguNTE4MzEwOCBMMjM4LjQ3MTY0OSwzOC41MTgzMTA4IFogTTM3LjM1ODY2MTQsMTU3LjM5NDYxNCBMMzcuMzI1MDA0LDEwNi40NzEwMTIgTDY1LjY1MzI5MDgsMTMxLjc2MDk1NiBMMzcuMzU4NjYxNCwxNTcuMzk0NjE0IFogTTEyNy45ODkyOTEsMTMxLjc0MjA4OCBMMTg0LjYyNDQ0Niw4OS40MDM2NjUzIEwxODQuNjQ2ODg0LDE3NC40MTE5ODQgTDEyNy45ODkyOTEsMTMxLjc0MjA4OCBaIiBmaWxsPSIjNjgyMTdBIj48L3BhdGg%2BDQogIDwvZz4NCjwvc3ZnPg%3D%3D)

![GCC](https://img.shields.io/badge/Linux-GCC_Not_sure-F34B7D?logo=linux&logoColor=white)
![Clang](https://img.shields.io/badge/macOS-Clang_Not_sure-000000?logo=apple&logoColor=white)

![Bilibili Name](https://img.shields.io/badge/Bilibili_Name-%E6%98%8E%E6%98%8E%E6%9C%88%E6%98%8E%E6%9C%8811019-8C0C02?logo=bilibili&logoColor=blue)
![Bilibili UID](https://img.shields.io/badge/Bilibili_UID-3707056078982013-ADFFF1?logo=data%3Aimage%2Fjpeg%3Bbase64%2C%2F9j%2F4AAQSkZJRgABAQEAYABgAAD%2F4QAiRXhpZgAATU0AKgAAAAgAAQESAAMAAAABAAEAAAAAAAD%2F2wBDAAIBAQIBAQICAgICAgICAwUDAwMDAwYEBAMFBwYHBwcGBwcICQsJCAgKCAcHCg0KCgsMDAwMBwkODw0MDgsMDAz%2F2wBDAQICAgMDAwYDAwYMCAcIDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAz%2FwAARCAAKAA4DASIAAhEBAxEB%2F8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL%2F8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4%2BTl5ufo6erx8vP09fb3%2BPn6%2F8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL%2F8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3%2BPn6%2F9oADAMBAAIRAxEAPwD9lfE%2Bl2XhfUNTSyi1G%2Bc3Ms7Qya28TZlOXX5p1yuS20cBQpA99TwN4Y0vV%2FECXElxrlvPZWskMcD6zKRtd42feDKdzKyKAw6BiM810Vl4I0XWfGV%2FcXmkaXdzocLJNaxyOowp4JGerMfxPrWho%2BkWlv421N47W3R2jQlljAJJ69u%2BB%2BVAH%2F%2FZ)

**作者：** [ 明明月明月11019 ]( https://space.bilibili.com/3707056078982013 )

**注意：** 本项目不保证绝对无 Bug 或严重漏洞，使用前请阅读 [ LICENSE 文件 ]( LICENSE )。

## 这是个什么东西？

`MyTools` 是一个纯 C 语言实现的单头文件（Header-Only）工具库。它的初衷是为了解决 C 语言标准库在日常开发中缺失的拼图（比如没有动态数组、`strtok` 极度难用、跨平台控制台开发繁琐等）。

你只需要把代码下载下来，把包含目录加到编译器里，直接 `#include "tools.h"` 就能用，不需要配置复杂的 CMake 或链接静态库。

## 特点

*   **单头文件 (Header-Only)**：函数均由 `static inline` 修饰，随引随用，不影响跨文件链接。
*   **命名空间隔离**：提供长短名双轨制。如果不想要短名字（如 `sum()`），只需定义 `TOOL_XXX_NO_SIMPLE_FUNC_NAME` 宏，就能强制使用带 `TOOL_` 前缀的安全版本，防止符号污染。
*   **底层性能**：没有过度封装。所有数据结构都是手动管理内存，哈希表实现了真正的 O(1) 遍历，宏生成的 Vector 性能直接对齐 C++ 模板。
*   **防御性编程**：带有跨平台的断言机制（包含行号和条件提示），以及强制检查返回值的 `TOOL_CHKRET` 宏。

## 模块目录

目前包含以下核心模块，你可以在对应目录下找到详细实现：

*   `toolsfamily.h`：核心基建。包含跨平台 Sleep、彩色日志、断言、返回值强制检查。
*   `Hash/`：哈希表。实现了墓碑机制、自动扩容缩容。包含带 O(1) 遍历特性的 `ToolHashMap`。
*   `Vector/`：
    *   `GeneralVector`：基于 `void*` 的泛型动态数组。
    *   `TemplateVector`：宏生成强类型数组（如 `TOOL_DEFINE_VECTOR(int, Int)`）。
*   `String/`：常用字符串处理。包括安全的切割（不破坏原串）、Trim、忽略大小写比较、前后缀匹配。
*   `Math/`：数学工具。万能求和、平均数、向下取整除法、2D 坐标压缩（Pack）等。
*   `Node/`：带哨兵节点的双向循环链表。
*   `Windows/`：（可选）Windows 专属。包括高精度计时器、完美 UTF-8/UTF-16 转换、控制台极速清屏与颜色控制。

## 快速上手

开箱即用，无需任何多余配置：

```c
#include "C/tools.h"
// 下面即可写代码
```

## 如何屏蔽短名（宏隔离）

如果你在一个大型项目里引入 MyTools，担心短名冲突，可以在 `#include "tools.h"` 之前定义相关的禁用宏：

```c
// 具体宏名以实际为主
#define TOOL_MATH_NO_SIMPLE_FUNC_NAME
#define TOOL_GENERAL_VECTOR_NO_SIMPLE_FUNC_NAME
#define TOOL_HASH_MAP_NO_SIMPLE_FUNC_NAME

#include "tools.h"
```

## 运行环境

*   **Windows**: Visual Studio 2019 及以上, MinGW (已实测通过)
*   **Linux / macOS**: GCC, Clang (未实测，理论上支持)

---

## 更新日志

### v0.0.5
*   **docs**：优化 README.md 排版。

### v0.0.4
*   **docs**: 优化 README.md 排版，添加自定义协议 `README.txt`。

### v0.0.3
*   **docs**: 优化 README.md 的表述。

### v0.0.2
*   **fix**: 修复 `ToolHashMap.h` 中的幽灵数据问题（墓碑机制下的延迟插入）。

### v0.0.1
*   **feat**: 首发版本。添加了主架构等核心模块。