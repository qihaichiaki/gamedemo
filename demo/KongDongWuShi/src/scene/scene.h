#ifndef _SCENE_H_
#define _SCENE_H_

#include <graphics.h>
#include <string>
#include <functional>


// scene 场景基类

class Scene
{
    using CheckSwitch = std::function<bool()>;
public:
    Scene() = default;
    ~Scene() = default;

    virtual void onEnter() { };
    virtual void onUpdate(float delta) { };
    virtual void onExit() { };

    virtual void onInput(const ExMessage&) { };
    virtual void onRender() { };

    void setSwitchScene(const std::string& scene) { nextScene = scene; }
    void registerCheckSwitch(CheckSwitch checkSwitch) { this->checkSwitch = checkSwitch; }

protected:
    std::string nextScene;
    CheckSwitch checkSwitch;  // 由外部传入多少时机进行切换场景
};

#endif