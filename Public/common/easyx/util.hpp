#ifndef _UTIL_H_
#define _UTIL_H_

#include <graphics.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../camera.hpp"

// 音频播放和绘图函数
#pragma comment(lib, "WINMM.lib")    // 音频播放
#pragma comment(lib, "MSIMG32.lib")  // AlphaBlend

/// @brief 矩形
/// @note x、y为矩形左上角坐标
/// @note w、h为矩形宽高
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
    // BlendOp混合操作 AC_SRC_OVER(将alpha信息从原位图放在目标位图上)
    // BlendFlags混合标记 0
    // SourceConstantAlpha位图透明度 255(不透明)
    // AlphaFormat AC_SRC_ALPHA->位图32位(存在alpha)
    static BLENDFUNCTION blendFunc = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    AlphaBlend(GetImageHDC(GetWorkingImage()), rectDst->x, rectDst->y, rectDst->w, rectDst->h,
               GetImageHDC(img), rectSrc ? rectSrc->x : 0, rectSrc ? rectSrc->y : 0,
               rectSrc ? rectSrc->w : img->getwidth(), rectSrc ? rectSrc->h : img->getheight(),
               blendFunc);
}

/// @brief 使用摄像机绘制透明图片
/// @param camera 相机对象
/// @param img 需要绘制的图片对象
/// @param rectDst 目标矩形(裁剪的图片贴附在窗口的哪一部分区域)
/// @param rectSrc 源矩形(目标图片上裁剪区域位置和大小)
inline void putimageEx(Camera camera, IMAGE* img, const Rect* rectDst,
                       const Rect* rectSrc = nullptr)
{
    const Vector2& posCamera = camera.getPosition();
    Rect rectDstCamera;
    rectDstCamera.x = rectDst->x - static_cast<int>(posCamera.x);
    rectDstCamera.y = rectDst->y - static_cast<int>(posCamera.y);
    rectDstCamera.w = rectDst->w;
    rectDstCamera.h = rectDst->h;

    putimageEx(img, &rectDstCamera, rectSrc);
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

/// @brief 播放音频
/// @param id 音频id
/// @param isLoop 是否循环
inline void playAudio(LPCTSTR id, bool isLoop = false)
{
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("play %s %s from 0"), id, isLoop ? _T("repeat") : _T(""));
    mciSendString(strCmd, NULL, 0, NULL);
}

/// @brief 停止音频
/// @param id 音频id
inline void stopAudio(LPCTSTR id)
{
    static TCHAR strCmd[512];
    _stprintf_s(strCmd, _T("stop %s"), id);
    mciSendString(strCmd, NULL, 0, NULL);
}

/// @brief 取[minNum, maxNum]之间任意随机数
/// @param minNum 最小值
/// @param maxNum 最大值
/// @return 区间内任意值
inline int rangeRandom(int minNum, int maxNum) { return minNum + rand() % (maxNum - minNum + 1); }

// 将 UTF-8 转换为 GBK 编码
inline std::string utf8_to_gbk(const std::string& utf8Str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], len);

    len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string gbkStr(len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &gbkStr[0], len, NULL, NULL);

    return gbkStr;
}

/// @brief 绘制字体以及灰色阴影
/// @param x 字体水平位置
/// @param y 字体竖直位置
/// @param str 文本
/// @param color 文本颜色，默认白色
/// @param colorShaded 文本阴影颜色，默认灰色
/// @note 根据是否UNICODE编码环境，处理字符编码
inline void outTextXYShaded(int x, int y, LPCTSTR str, int size = 0,
                            COLORREF color = RGB(255, 255, 255),
                            COLORREF colorShaded = RGB(45, 45, 45))
{
    // 设置字体样式
    if (size > 0)
        settextstyle(size, 0, _T("zpix"));
    else
        settextstyle(25, 0, _T("zpix"));

        // 设置字体颜色和输出字体
#ifdef _UNICODE
    settextcolor(colorShaded);
    outtextxy(x + 3, y + 3, str);
    settextcolor(color);
    outtextxy(x, y, str);
#else
    settextcolor(colorShaded);
    outtextxy(x + 3, y + 3, utf8_to_gbk(str).c_str());
    settextcolor(color);
    outtextxy(x, y, utf8_to_gbk(str).c_str());
#endif
}

struct StrData
{
    StrData() = default;
    StrData(const std::string& str) : _str(str) {}

    bool empty() const { return _str == ""; }
    int Int() const { return std::stoi(_str); }

    std::string _str;
};

// 数据转换，序列化和反序列化的提供类
class DataTransformation
{
public:
    DataTransformation() = default;
    DataTransformation(const std::string& path) { load(path); }

    // 加载文件
    bool load(const std::string& path)
    {
        std::ifstream readStream(path);
        std::string line;

        if (!readStream.is_open()) return false;

        while (std::getline(readStream, line)) {
            if (line == "" || line[0] == '#') continue;

            std::string value[2];
            int index = 0;
            for (char c : line) {
                if (c == '=')
                    ++index;
                else if (c != ' ')
                    value[index].push_back(c);
            }

            _map.emplace(value[0], StrData(value[1]));
            // debug
            std::cout << "input: " << value[0] << " = " << value[1] << std::endl;
        }

        readStream.close();
        return true;
    }

    StrData& operator[](const std::string& key) { return _map[key]; }

private:
    std::unordered_map<std::string, StrData> _map;
};

#endif