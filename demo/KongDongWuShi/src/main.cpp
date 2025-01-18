#pragma comment (linker,"/subsystem:windows /entry:mainCRTStartup")		// 关闭控制台窗口

#include "util.hpp"
#include "resourcesManager.h"
#include "sceneManager.h"

#include <chrono>
#include <future>
#include <graphics.h>
#include <iostream>
#include <thread>

using std::chrono::nanoseconds;
using std::chrono::steady_clock;
using std::chrono::duration;


int main(int argc, char const *argv[])
{
    // 设置随机数种子，确保每次随机都能在意义层面上的随机
    srand((unsigned int)time(0));

    // HWND hwnd = initgraph(1280, 720);
    HWND hwnd = initgraph(1280, 720, EX_SHOWCONSOLE);  // 显示控制台窗口
    // SetWindowText(hwnd, _T("Hollow Katana"));
    SetWindowTextW(hwnd, L"空洞武士");

    // 资源加载
    // try
    // {
    //     ResourcesManager::instance()->load();
    // }
    // catch(const LPCTSTR id)
    // {
    //     TCHAR errMsg[512];
    //     _stprintf_s(errMsg, _T("Unable to load: %s"), id);
    //     MessageBox(hwnd, errMsg, _T("Resource loading failure"), MB_OK | MB_ICONERROR);
    //     exit(-1);
    // }

    // 加载前置
    // FIXBUG: 加载字体分给线程会让当前过渡场景没有设置字体
    // BUG: 音频加载播放分线程会出现问题，当前解决方案为加载放在主线程
    ResourcesManager::instance()->loadPre();

    std::future<void> loadFuture = std::async(std::launch::async, [&]()
        {
            try
            {
                ResourcesManager::instance()->loadAsync();
            }
            catch(const LPCTSTR id)
            {
                TCHAR errMsg[512];
                _stprintf_s(errMsg, _T("Unable to load: %s"), id);
                MessageBox(hwnd, errMsg, _T("Resource loading failure"), MB_OK | MB_ICONERROR);
                exit(-1);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1400));
        });

    // 场景初始化
    SceneManager::instance()->setEntryScene("menu");
    SceneManager::instance()->loadSwitchScene(loadFuture, "menu");

    // 144 FPS
    const nanoseconds frameDuration((int)1e9 / 144);
    steady_clock::time_point lastTick = steady_clock::now();

    ExMessage msg;
    bool isQuit = false;

    BeginBatchDraw();

    while(!isQuit)
    {
        // 处理消息
        while(peekmessage(&msg))
            SceneManager::instance()->onInput(msg);

        steady_clock::time_point frameStart = steady_clock::now();
        duration<float> delta = duration<float>(frameStart - lastTick);

        SceneManager::instance()->onUpdate(delta.count());

        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        // 处理绘图
        SceneManager::instance()->onRender();

        FlushBatchDraw();

        lastTick = frameStart;
        nanoseconds sleepDuration = frameDuration - (steady_clock::now() - frameStart);
        if (sleepDuration > nanoseconds(0))
            std::this_thread::sleep_for(sleepDuration);
    }

    EndBatchDraw();
    closegraph();

    return 0;
}
