#ifndef _UTIL_H_
#define _UTIL_H_

#include <graphics.h>
#include <cstdio>

// 音频播放和绘图函数
#pragma comment(lib, "WINMM.lib")
#pragma comment(lib, "MSIMG32.lib")

struct Rect
{
    int x, y;
    int w, h;
};

/// @brief 绘制透明图片
/// @param img 需要绘制的图片对象
/// @param rectDst 目标矩形(裁剪的图片贴附在窗口的哪一部分区域)
/// @param rectSrc 源矩形(目标图片上裁剪区域位置和大小)
inline void putimageEx(IMAGE* img, const Rect* rectDst, const Rect* rectSrc = nullptr)
{
    static BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(GetImageHDC(GetWorkingImage()), rectDst->x, rectDst->y, rectDst->w, rectDst->h,
        GetImageHDC(img), rectSrc ? rectSrc->x : 0, rectSrc? rectSrc->y : 0,
        rectSrc? rectSrc->w : img->getwidth(), rectSrc? rectSrc->h : img->getheight(), blendFunc);
}

// 音频资源处理
// 根据传入参数，通过_stprintf_s格式化为对应的指令，然后传给MCI进行处理

/// @brief 加载音频
/// @param path 资源路径
/// @param id 资源id
inline void loadAudio(LPCTSTR path, LPCTSTR id)
{
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("open %s alias %s"), path, id);
    mciSendString(strCmd, NULL, 0, NULL);
}

inline void playAudio(LPCTSTR id, bool isLoop = false)
{
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("play %s %s from 0"), id, isLoop ? _T("repeat") : _T(""));
    mciSendString(strCmd, NULL, 0, NULL);
}

inline void stopAudio(LPCTSTR id)
{
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("stop %s"), id);
    mciSendString(strCmd, NULL, 0, NULL);
}

inline int rangeRandom(int minNum, int maxNum)
{
    return minNum + rand() % (maxNum - minNum + 1);
}

#endif