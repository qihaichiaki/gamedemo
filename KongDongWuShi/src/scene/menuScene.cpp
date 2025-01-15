#include "menuScene.h"
#include "util.hpp"
#include "sceneManager.h"
#include "resourcesManager.h"

#include <string>

MenuScene::MenuScene()
{
    Atlas* atlas = ResourcesManager::instance()->findAtlas("bug");

    bugs.resize(bugMaxSize);
    for (auto& bug: bugs)
    {
        bug.addFrame(atlas);
        bug.setInterval(0.01f * (float)(rangeRandom(20, 30)));
        bug.setLoop(true);

        bug.setPosition({ (float)(rangeRandom(300, 980)), (float)(rangeRandom(50, 250)) });
    }

    light.addFrame(ResourcesManager::instance()->findAtlas("light"));
    light.setInterval(0.15f);
    light.setLoop(true);
    light.setPosition({ 640.0f, 400.0f });

    ui_choose[0].addFrame(ResourcesManager::instance()->findAtlas("ui_choose_left"));
    ui_choose[1].addFrame(ResourcesManager::instance()->findAtlas("ui_choose_right"));
    ui_choose[0].setInterval(0.15f);
    ui_choose[1].setInterval(0.15f);
    ui_choose[0].setLoop(false);
    ui_choose[1].setLoop(false);
    baseWidth = 530.0f;
    baseHeight = 520.0f;
    ui_choose[0].setPosition({baseWidth, baseHeight});
    ui_choose[1].setPosition({baseWidth + buttonSpacing, baseHeight});
}

void MenuScene::onEnter()
{
    playAudio("bgm_start", true);
}

void MenuScene::onUpdate(float delta)
{
    for (auto& bug: bugs)
        bug.onUpdate(delta);
    light.onUpdate(delta);
    ui_choose[0].onUpdate(delta);
    ui_choose[1].onUpdate(delta);
}

// 定位ui
void MenuScene::__positioningUIChoose()
{
    playAudio("ui_switch");
    ui_choose[0].setPosition({ baseWidth, baseHeight + interval * selectIndex});
    ui_choose[1].setPosition({ baseWidth + buttonSpacing, baseHeight + interval * selectIndex});
    ui_choose[0].reset();
    ui_choose[1].reset();
}

void MenuScene::onInput(const ExMessage& msg)
{
    switch (msg.message)
    {
    case WM_KEYDOWN:
        switch (msg.vkcode)
        {
        case VK_UP:
        case 0x57:
            selectIndex = selectIndex <= 0 ? 2 : selectIndex - 1;
            __positioningUIChoose();
            break;
        case VK_DOWN:
        case 0x53:
            selectIndex = selectIndex >= 2 ? 0 : selectIndex + 1;
            __positioningUIChoose();
            break;
        case VK_RETURN:
            playAudio("ui_confirm");
            if (selectIndex == 0)
                SceneManager::instance()->switchScene("game");
            else if (selectIndex == 1)
                exit(0);
            else if (selectIndex == 2)
                SceneManager::instance()->switchScene("setting");
            break;
        default:
            break;
        }


        break;
    default:
        break;
    }


}

void MenuScene::onRender()
{
    // 渲染文字
    outTextXYShaded(400, 100, _T("空洞武士"), 120);

    COLORREF color[3];
    for (int i = 0; i < 3; i++)
        color[i] = RGB(255, 255, 255);
    
    color[selectIndex] = RGB(255, 105, 25);

    outTextXYShaded(565, 500, _T("开始游戏"), 35, color[0]);
    outTextXYShaded(565, 550, _T("结束游戏"), 35, color[1]); // 35 * 4 = 150/2 = 75
    outTextXYShaded(565, 600, _T("按键设置"), 35, color[2]);

    // 渲染动画
    for (auto& bug: bugs)
        bug.onRender();
    light.onRender();
    ui_choose[0].onRender();
    ui_choose[1].onRender();
}

void MenuScene::onExit()
{
    stopAudio("bgm_start");
}

std::string SettingScene::keyCodeStr(int keyCode)
{
    std::string str;
    switch (keyCode)
    {
    case 0x4A:
        str = "J";
        break;
    case 0x49:
        str = "I";
        break;
    case 0x10:
        str = "LSHIFT";
        break;
    case VK_SPACE:
        str = "SPACE";
        break;
    case 0x57:
        str = "W";
        break;
    case 0x53:
        str = "S";
        break;
    case 0x41:
        str = "A";
        break;
    case 0x44:
        str = "D";
        break;
    default:
        str = "?";
        break;
    }

    return str;
}

// Setting Scene
void SettingScene::onRender()
{
    std::string attackStr = "attack: " + keyCodeStr(ResourcesManager::instance()->findSetData("attack").Int());
    std::string bulletTimeStr = "bulletTime: " + keyCodeStr(ResourcesManager::instance()->findSetData("bulletTime").Int());
    std::string rollStr = "roll: " + keyCodeStr(ResourcesManager::instance()->findSetData("roll").Int());
    std::string jumpStr = "jump: " + keyCodeStr(ResourcesManager::instance()->findSetData("jump").Int());
    std::string upStr = "up: " + keyCodeStr(ResourcesManager::instance()->findSetData("up").Int());
    std::string downStr = "down: " + keyCodeStr(ResourcesManager::instance()->findSetData("down").Int());
    std::string leftStr = "left: " + keyCodeStr(ResourcesManager::instance()->findSetData("left").Int());
    std::string rightStr = "right: " + keyCodeStr(ResourcesManager::instance()->findSetData("right").Int());

    // 渲染当前的所有用户设置
    outTextXYShaded(100, 100, attackStr.c_str(), 35);
    outTextXYShaded(100, 140, bulletTimeStr.c_str(), 35);
    outTextXYShaded(100, 180, rollStr.c_str(), 35);
    outTextXYShaded(100, 220, jumpStr.c_str(), 35);
    outTextXYShaded(100, 260, attackStr.c_str(), 35);
    outTextXYShaded(100, 300, upStr.c_str(), 35);
    outTextXYShaded(100, 340, downStr.c_str(), 35);
    outTextXYShaded(100, 380, leftStr.c_str(), 35);
    outTextXYShaded(100, 420, rightStr.c_str(), 35);

}

void SettingScene::onInput(const ExMessage& msg)
{
    if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE)
        SceneManager::instance()->switchScene("menu");
}

void SettingScene::onExit()
{

}