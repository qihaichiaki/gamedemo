#include "loadScene.h"
#include "util.hpp"
#include "sceneManager.h"
#include "resourcesManager.h"

LoadScene::LoadScene()
{
    loadSceneAnimation.setLoop(true);
    loadSceneAnimation.setInterval(0.12f);
    loadSceneAnimation.addFrame(ResourcesManager::instance()->findAtlas("little_knight"));
    loadSceneAnimation.setPosition({ 640.0f, 360.0f });
}

void LoadScene::onEnter()
{
    playAudio("play_tape", true);
}

void LoadScene::onUpdate(float delta)
{
    loadSceneAnimation.onUpdate(delta);
    if (checkSwitch())
        SceneManager::instance()->switchScene(nextScene);
}

void LoadScene::onRender()
{
    loadSceneAnimation.onRender();
}

void LoadScene::onExit()
{
    stopAudio("play_tape");
}