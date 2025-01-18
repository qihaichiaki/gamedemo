#include "sceneManager.h"
#include "scene/menuScene.h"
#include "scene/gameScene.h"
#include "scene/loadScene.h"

#include <iostream>
#include <chrono>

SceneManager* SceneManager::instance()
{
    static SceneManager manager;
    return &manager;
}

SceneManager::SceneManager()
{
    // 加载当前当前场景资源
    sceneTool["menu"] = new MenuScene();
    sceneTool["setting"] = new SettingScene();
    sceneTool["game"] = new GameScene();
    sceneTool["load"] = new LoadScene();
}

SceneManager::~SceneManager()
{
    for (auto& scene: sceneTool)
    {
        delete scene.second;
    }
}

void SceneManager::onInput(const ExMessage& msg)
{
    currentScene->onInput(msg);
}

void SceneManager::setEntryScene(const std::string& scene)
{
    if (sceneTool.find(scene) == sceneTool.end())
        return;  // TODO: 异常处理?
    currentScene = sceneTool[scene];
    currentSceneId = scene;
    currentScene->onEnter();
}

void SceneManager::switchScene(const std::string& scene)
{
    if (sceneTool.find(scene) == sceneTool.end())
        return;  // TODO: 异常处理?
    
    currentScene->onExit();
    currentScene = sceneTool[scene];
    currentSceneId = scene;
    currentScene->onEnter();
}

void SceneManager::loadSwitchScene(std::future<void>& t, const std::string& scene)
{
    currentScene->onExit();
    currentScene = sceneTool["load"];
    currentSceneId = "load";
    currentScene->setSwitchScene(scene);
    // 检查加载资源线程是否执行完毕
    currentScene->registerCheckSwitch([&]()
        {
            if (t.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                return true;
            return false;
        });
    currentScene->onEnter();
}