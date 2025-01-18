#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "scene/scene.h"

#include "string"
#include <future>
#include "unordered_map"

// 场景管理器类

class SceneManager
{
public:
    static SceneManager* instance();

    /// @brief 设置默认第一个进入场景
    /// @param  
    void setEntryScene(const std::string&);

    /// @brief 切换场景
    /// @param  
    void switchScene(const std::string&);

    /// @brief 加载过渡切换场景
    /// @param t 过渡过程中资源加载的线程(注意保证可重入)
    /// @param scene 过渡结束进入的场景
    void loadSwitchScene(std::future<void>&, const std::string&);

    void onInput(const ExMessage&);
    void onUpdate(float delta) { currentScene->onUpdate(delta); };
    void onRender() { currentScene->onRender(); }

private:
    Scene* currentScene = nullptr;
    std::string currentSceneId;
    std::unordered_map<std::string, Scene*> sceneTool;  // 场景池
private:
    SceneManager();
    SceneManager(const SceneManager&) = delete;
    ~SceneManager();
};

#endif