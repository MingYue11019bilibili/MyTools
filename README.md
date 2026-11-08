# MyTools - 一个好用的 C 语言单头文件工具库

**作者：** [ 明明月明月11019 ]( https://space.bilibili.com/3707056078982013 )

![C Standard](https://img.shields.io/badge/C-C99%2FC11-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![Made by](https://img.shields.io/badge/Made_by-MingYue11019bilibili-green)

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
>   Windows: Visual Studio 2019 及以上, MinGW
>   Linux / macOS: GCC, Clang

---

# 更新日志
> 0.0.1 首发
1. 添加了主架构等核心模块