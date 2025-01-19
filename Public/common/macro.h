#ifndef _MACRO_H_
#define _MACRO_H_

/**
 * 定义一些公共的宏之类的
 */

// 设置windows控制台输出/输入编码为 UTF-8
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_USE_UTF8         \
    SetConsoleOutputCP(CP_UTF8); \
    SetConsoleCP(CP_UTF8)
#else
#define WINCMD_USE_UTF8
#endif

#endif