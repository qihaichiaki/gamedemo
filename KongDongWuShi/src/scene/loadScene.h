#ifndef __LOAD_SCENE_H__
#define __LOAD_SCENE_H__

#include "scene.h"
#include "animation.h"

// 过渡场景

class LoadScene : public Scene
{
public:
    LoadScene();
    ~LoadScene() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onRender() override;
    void onExit() override;

private:
    Animation loadSceneAnimation;
};


#endif