#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "scene.h"

// 菜单场景

class GameScene : public Scene
{
public:
    GameScene() = default;
    ~GameScene() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;

    void onInput(const ExMessage& msg) override;
    void onRender() override;
};


#endif