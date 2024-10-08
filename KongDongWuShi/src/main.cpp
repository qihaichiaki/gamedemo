#pragma comment (linker,"/subsystem:windows /entry:mainCRTStartup")		// 关闭控制台窗口

#include "util.h"
#include "resourcesManager.h"
#include "collisionManager.h"
#include "characterManager.h"
#include "bulletTimeManager.h"

#include <chrono>
#include <thread>
#include <graphics.h>
#include <iostream>

using std::chrono::nanoseconds;
using std::chrono::steady_clock;
using std::chrono::duration;

static void drawRemainHp()
{
    static IMAGE* imgUiHeart = ResourcesManager::instance()->findImage("ui_heart");
    Rect rectDst = { 0, 10, imgUiHeart->getwidth(), imgUiHeart->getheight() };
    for (int i = 0; i < CharacterManager::instance()->getPlayer()->getHp(); ++i)
    {
        rectDst.x = 10 + i * 40;
        putimageEx(imgUiHeart, &rectDst);
    }
}

static void drawBackground()
{
    static IMAGE* imgBackground = ResourcesManager::instance()->findImage("background");
    static Rect rectDst = 
    {
        (getwidth() - imgBackground->getwidth()) / 2,
        (getheight() - imgBackground->getheight()) /2,
        imgBackground->getwidth(),
        imgBackground->getheight()
    };

    putimageEx(imgBackground, &rectDst);
}

int main(int argc, char const *argv[])
{
    HWND hwnd = initgraph(1280, 720);
    // SetWindowText(hwnd, _T("Hollow Katana"));
    SetWindowTextW(hwnd, L"空洞武士");

    // 资源加载
    try
    {
        ResourcesManager::instance()->load();
    }
    catch(const LPCTSTR id)
    {
        TCHAR errMsg[512];
        _stprintf_s(errMsg, _T("Unable to load: %s"), id);
        MessageBox(hwnd, errMsg, _T("Resource loading failure"), MB_OK | MB_ICONERROR);
        return -1;
    }

    // bgm
    playAudio(_T("bgm"), true);

    // 游戏说明
    MessageBoxW(GetHWnd(), L"左: a 右: d\n跳: space 攻击: j\n闪避: lshift 子弹时间: i", L"按键映射", MB_OK);

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
            CharacterManager::instance()->onInput(msg);

        steady_clock::time_point frameStart = steady_clock::now();
        duration<float> delta = duration<float>(frameStart - lastTick);

        // 处理更新
        CharacterManager::instance()->onUpdate(delta.count());
        CollisionManager::instance()->processCollide();

        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        // 处理绘图
        drawBackground();
        CharacterManager::instance()->onRender();
        // CollisionManager::instance()->onDebugRender();
        drawRemainHp();

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
