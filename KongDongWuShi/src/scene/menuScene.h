#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "animation.hpp"
#include "scene.h"

// 菜单场景
class MenuScene : public Scene
{
public:
    MenuScene();
    ~MenuScene() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onInput(const ExMessage& msg) override;
    void onRender() override;
    void onExit() override;

private:
    void __positioningUIChoose();

private:
    std::vector<Animation> bugs;
    Animation light;
    const int bugMaxSize = 10;

    // ui_choose
    Animation ui_choose[2];
    int selectIndex = 0;
    float baseWidth;
    float baseHeight;
    const float buttonSpacing = 210.0f;
    const float interval = 50.0f;
};

// Setting Scene
class SettingScene : public Scene
{
public:
    SettingScene() = default;
    ~SettingScene() = default;

    void onInput(const ExMessage& msg) override;
    void onRender() override;
    void onExit() override;

public:
    // 将keycode转换为字符串的形式
    static std::string keyCodeStr(int keyCode);
};

#endif