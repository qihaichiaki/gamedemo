#include "gameScene.h"
#include "util.hpp"
#include "resourcesManager.h"
#include "characterManager.h"
#include "collisionManager.h"

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

void GameScene::onEnter()
{
    // game bgm
    playAudio(_T("bgm"), true);
    // 游戏说明
    // MessageBoxW(GetHWnd(), L"左: a 右: d\n跳: space 攻击: j\n闪避: lshift 子弹时间: i", L"按键映射", MB_OK);
}

void GameScene::onUpdate(float delta)
{
    // 处理更新
    CharacterManager::instance()->onUpdate(delta);
    CollisionManager::instance()->processCollide();
}

void GameScene::onInput(const ExMessage& msg)
{
    CharacterManager::instance()->onInput(msg);
}

void GameScene::onRender()
{
    drawBackground();
    CharacterManager::instance()->onRender();
    // CollisionManager::instance()->onDebugRender();
    drawRemainHp();
}

void GameScene::onExit()
{
    stopAudio(_T("bgm"));
}