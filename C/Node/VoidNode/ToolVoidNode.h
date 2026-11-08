// Node/VoidNode/ToolsVoidNode.h

#pragma once

#ifndef MMYMYS_TOOLS_VOID_NODE_H
#define MMYMYS_TOOLS_VOID_NODE_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../toolsfamily.h"

#ifndef TOOL_VOID_NODE_NO_SIMPLE_STRUCT_NAME
#define TOOL_VOID_NODE_TYPE_ENUM_NAME VoidNodeType
#define TOOL_VOID_NODE_STRUCT_NAME    VoidNode
#else
#define TOOL_VOID_NODE_TYPE_ENUM_NAME TOOL_VoidNodeType
#define TOOL_VOID_NODE_STRUCT_NAME    TOOL_VoidNode
#endif // TOOL_VOID_NODE_NO_SIMPLE_STRUCT_NAME

#ifndef TOOL_VOID_NODE_NO_SIMPLE_FUNC_NAME
#define TOOL_NODE_CREATE_HEAD       createNewVoidNodeHead
#define TOOL_NODE_LIST              listVoidNode
#define TOOL_NODE_NEW_NO_TYPES      newVoidNodeHeadNoTypes
#define TOOL_NODE_NEW_TYPES         newVoidNodeHeadTypes
#define TOOL_NODE_PRINT_DATA        printVoidNodeData
#define TOOL_NODE_REMOVE_ALL        removeAllVoidNodeImpl
#define removeAllVoidNode(head)     removeAllVoidNodeImpl(head)
#else
#define TOOL_NODE_CREATE_HEAD       toolCreateNewVoidNodeHead
#define TOOL_NODE_LIST              toolListVoidNode
#define TOOL_NODE_NEW_NO_TYPES      toolNewVoidNodeHeadNoTypes
#define TOOL_NODE_NEW_TYPES         toolNewVoidNodeHeadTypes
#define TOOL_NODE_PRINT_DATA        toolPrintVoidNodeData
#define TOOL_NODE_REMOVE_ALL        toolRemoveAllVoidNodeImpl
#define toolRemoveAllVoidNode(head) toolRemoveAllVoidNodeImpl(head)
#endif // TOOL_VOID_NODE_NO_SIMPLE_FUNC_NAME

// 头插法创建节点宏魔法
#define TOOL_EXPAND_N(Parameter) Parameter        // 由于老版MSVC的印度程序员写的 __VA_ARGS__ 会作为完整参数，所以需要展开解析
#define TOOL_GET_NAME_4_N(_1, _2, _3, _4, ...) _4 // 命中第四个值
#define TOOL_newVoidNodeHead_1(head) TOOL_NODE_NEW_NO_TYPES(head)                                                                                                  // 创建普通节点，无类型和值
#ifndef TOOL_VOID_NODE_NO_PARAMETER_WARN
#define TOOL_newVoidNodeHeadParameterError(...) (printf("newVoidNodeHead 参数错误：应传入 1 或 3 个实参，但只得到 2 个参数\n"), (TOOL_VOID_NODE_STRUCT_NAME*)NULL) // 不支持两个参数
#else
#define TOOL_newVoidNodeHeadParameterError(...) (TOOL_VOID_NODE_STRUCT_NAME*)NULL                                                                                  // 不支持两个参数
#endif // TOOL_VOID_NODE_NO_ParameterError
#define TOOL_newVoidNodeHead_3(head, type, data) TOOL_NODE_NEW_TYPES(head, type, data)                                                                             // 创建有类型和值的节点
#ifndef TOOL_VOID_NODE_NO_SIMPLE_FUNC_NAME
#define newVoidNodeHead(...) TOOL_EXPAND_N(TOOL_GET_NAME_4_N(__VA_ARGS__, TOOL_newVoidNodeHead_3, TOOL_newVoidNodeHeadParameterError, TOOL_newVoidNodeHead_1)(__VA_ARGS__))     // 总宏魔法
#else
#define toolNewVoidNodeHead(...) TOOL_EXPAND_N(TOOL_GET_NAME_4_N(__VA_ARGS__, TOOL_newVoidNodeHead_3, TOOL_newVoidNodeHeadParameterError, TOOL_newVoidNodeHead_1)(__VA_ARGS__)) // 总宏魔法
#endif // TOOL_VOID_NODE_NO_SIMPLE_FUNC_NAME

// 类型枚举
typedef enum {
    TOOL_TYPE_I08, TOOL_TYPE_I16, TOOL_TYPE_I32, TOOL_TYPE_I64, // 整形
    TOOL_TYPE_U08, TOOL_TYPE_U16, TOOL_TYPE_U32, TOOL_TYPE_U64, // 无符号整形
    TOOL_TYPE_F32, TOOL_TYPE_F64, // 浮点
    TOOL_TYPE_C08                 // 字符
} TOOL_VOID_NODE_TYPE_ENUM_NAME;

// 节点结构体
typedef struct TOOL_VOID_NODE_STRUCT_NAME {
    TOOL_VOID_NODE_TYPE_ENUM_NAME type;     // 类型
    union {                // 数据域
        int8_t   i08;
        int16_t  i16;
        int32_t  i32;
        int64_t  i64;
        uint8_t  u08;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float    f32;
        double   f64;
        char     c08;
    } data;
    struct TOOL_VOID_NODE_STRUCT_NAME* prev; // 指针域：前驱
    struct TOOL_VOID_NODE_STRUCT_NAME* next; // 指针与：后继
} TOOL_VOID_NODE_STRUCT_NAME;

// 新建新链表
TOOLFUNC__ TOOL_VOID_NODE_STRUCT_NAME* TOOL_NODE_CREATE_HEAD() {
    TOOL_VOID_NODE_STRUCT_NAME* head = (TOOL_VOID_NODE_STRUCT_NAME*)malloc(sizeof(TOOL_VOID_NODE_STRUCT_NAME)); // 新建哨兵头，使用 malloc 申请新内存
    if (!head) return NULL; // 如果申请失败，返回 NULL
    head->type = TOOL_TYPE_C08; // 类型
    head->data.c08 = 'h'; // 值
    head->prev = head; // 双循环链表：前驱指向自己
    head->next = head; // 双循环链表：后继指向自己
    return head; // 返回链表头
}

// 打印链表某一节
TOOLFUNC__ void TOOL_NODE_PRINT_DATA(TOOL_VOID_NODE_STRUCT_NAME* node, const char* mode) {
    bool useDefaultMode = false;
    // 由于这个很少用并且仅为TOOL内部使用，所以用完就取消定义
#define TOOL_TOKEN_TO_STR_IMPL___(token) #token
#define TOOL_TOKEN_TO_STR___(token) TOOL_TOKEN_TO_STR_IMPL___(token)
    TOOL_ASSERT(node, TOOL_TOKEN_TO_STR___(TOOL_NODE_PRINT_DATA) ":此类型链表为双循环链表，节点指针不可能为NULL");
#undef TOOL_TOKEN_TO_STR___
#undef TOOL_TOKEN_TO_STR_IMPL___
    // 防呆设计：如果没传分隔符，默认用空格
    if (!mode) useDefaultMode = true;
    switch (node->type) {
    case TOOL_TYPE_I08:
        printf("%" PRId8 "%s", node->data.i08, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_I16:
        printf("%" PRId16 "%s", node->data.i16, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_I32:
        printf("%" PRId32 "%s", node->data.i32, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_I64:
        printf("%" PRId64 "%s", node->data.i64, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_U08:
        printf("%" PRIu8 "%s", node->data.u08, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_U16:
        printf("%" PRIu16 "%s", node->data.u16, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_U32:
        printf("%" PRIu32 "%s", node->data.u32, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_U64:
        printf("%" PRIu64 "%s", node->data.u64, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_F32:
        printf("%f%s", node->data.f32, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_F64:
        printf("%f%s", node->data.f64, useDefaultMode ? " " : mode);
        break;
    case TOOL_TYPE_C08:
        printf("%c%s", node->data.c08, useDefaultMode ? " " : mode);
        break;
    default:
        printf("Error:UnknownType%s", useDefaultMode ? " " : mode);
        break;
    }
}

// 打印全部链表
TOOL_CHKRET TOOLFUNC__ int32_t TOOL_NODE_LIST(TOOL_VOID_NODE_STRUCT_NAME* head, int32_t mode, const char* str) {
    if (!head)return -1; // 头指针不可用
    if (mode == 1) { // 正序
        TOOL_VOID_NODE_STRUCT_NAME* now = head->next; // 指定指针 now 为哨兵头的后继
        while (now != head) { // 由于是双循环链表， next 或 prev 不会为 NULL 所以要判断目前是不是哨兵头
            TOOL_NODE_PRINT_DATA(now, str); // 打印链表某一节
            now = now->next; // 更新
        }
        return 0;
    }
    else if (mode == -1) { // 倒序
        TOOL_VOID_NODE_STRUCT_NAME* now = head->prev; // 指定指针 now 为哨兵头的前驱
        while (now != head) { // 由于是双循环链表， next 或 prev 不会为 NULL 所以要判断目前是不是哨兵头
            TOOL_NODE_PRINT_DATA(now, str); // 打印链表某一节
            now = now->prev; // 更新
        }
        return 0;
    }
    return 1; // 未知模式
}

// 新建普通节点
TOOLFUNC__ TOOL_VOID_NODE_STRUCT_NAME* TOOL_NODE_NEW_NO_TYPES(TOOL_VOID_NODE_STRUCT_NAME* head) {
    if (!head) return NULL; // 若指针为空，返回 NULL
    TOOL_VOID_NODE_STRUCT_NAME* newVoidNode = (TOOL_VOID_NODE_STRUCT_NAME*)malloc(sizeof(TOOL_VOID_NODE_STRUCT_NAME)); // 新建节点，使用 malloc 申请新内存
    if (!newVoidNode) return NULL; // 申请失败，返回 NULL
    newVoidNode->type = TOOL_TYPE_C08;
    newVoidNode->data.c08 = '\0';
    // 头插
    newVoidNode->prev = head;
    newVoidNode->next = head->next;
    head->next->prev = newVoidNode;
    head->next = newVoidNode;
    return newVoidNode; // 返回新节点
}

// 新建有类型有值的节点
TOOLFUNC__ TOOL_VOID_NODE_STRUCT_NAME* TOOL_NODE_NEW_TYPES(TOOL_VOID_NODE_STRUCT_NAME* head, TOOL_VOID_NODE_TYPE_ENUM_NAME type, ...) {
    if (!head) return NULL; // 若指针为空，返回 NULL
    TOOL_VOID_NODE_STRUCT_NAME* newVoidNode = (TOOL_VOID_NODE_STRUCT_NAME*)malloc(sizeof(TOOL_VOID_NODE_STRUCT_NAME)); // 新建节点，使用 malloc 申请新内存
    if (!newVoidNode) return NULL; // 申请失败，返回 NULL
    newVoidNode->type = type; // 继承类型
    va_list typeData; // 读取可变参数
    va_start(typeData, type); // 初始化
    // 数据输入，因为函数传入参数会类型提升，部分需要强转
    switch (type) {
    case TOOL_TYPE_I08:
        newVoidNode->data.i08 = (int8_t)va_arg(typeData, int);
        break;
    case TOOL_TYPE_I16:
        newVoidNode->data.i16 = (int16_t)va_arg(typeData, int);
        break;
    case TOOL_TYPE_I32:
        newVoidNode->data.i32 = va_arg(typeData, int32_t);
        break;
    case TOOL_TYPE_I64:
        newVoidNode->data.i64 = va_arg(typeData, int64_t);
        break;
    case TOOL_TYPE_U08:
        newVoidNode->data.u08 = (uint8_t)va_arg(typeData, int);
        break;
    case TOOL_TYPE_U16:
        newVoidNode->data.u16 = (uint16_t)va_arg(typeData, int);
        break;
    case TOOL_TYPE_U32:
        newVoidNode->data.u32 = va_arg(typeData, uint32_t);
        break;
    case TOOL_TYPE_U64:
        newVoidNode->data.u64 = va_arg(typeData, uint64_t);
        break;
    case TOOL_TYPE_F32:
        newVoidNode->data.f32 = (float)va_arg(typeData, double);
        break;
    case TOOL_TYPE_F64:
        newVoidNode->data.f64 = va_arg(typeData, double);
        break;
    case TOOL_TYPE_C08:
        newVoidNode->data.c08 = (char)va_arg(typeData, int);
        break;
        //类型错误
    default:
        va_end(typeData); // 与 va_start 配对，释放可能的内存
        free(newVoidNode); // 释放内存
        return NULL;
    }
    va_end(typeData); // 与 va_start 配对，释放可能的内存
    // 头插
    newVoidNode->prev = head;
    newVoidNode->next = head->next;
    head->next->prev = newVoidNode;
    head->next = newVoidNode;
    return newVoidNode; // 返回新节点
}

// 删除所有节点
TOOLFUNC__ void TOOL_NODE_REMOVE_ALL(TOOL_VOID_NODE_STRUCT_NAME** phead) {
    if (!phead || !*phead) return;
    TOOL_VOID_NODE_STRUCT_NAME* head = *phead;
    *phead = NULL;
    if (!head) return; // 若指针为空，返回
    TOOL_VOID_NODE_STRUCT_NAME* now = head->next; // 指定指针 now 为哨兵头的后继
    while (now != head) { // 由于是双循环链表， next 或 prev 不会为 NULL 所以要判断目前是不是哨兵头
        TOOL_VOID_NODE_STRUCT_NAME* nxt = now->next; // 先继承 next 防止指针丢失
        free(now); // 释放内存，避免泄露
        now = nxt; // 继承指针，开始下一轮
    }
    free(head); // 释放内存，避免泄露
}

#endif // MMYMYS_TOOLS_VOID_NODE_H