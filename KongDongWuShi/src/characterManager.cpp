#include "player.h"
#include "enemy.h"
#include "characterManager.h"
#include "bulletTimeManager.h"


CharacterManager* CharacterManager::instance()
{
    static CharacterManager manager;

    return &manager;
}

void CharacterManager::onInput(const ExMessage& msg)
{
    player->onInput(msg);
}

void CharacterManager::onUpdate(float delta)
{
    player->onUpdate(delta);
    enemy->onUpdate(delta);
}

void CharacterManager::onRender()
{
    enemy->onRender();
    BulletTimeManager::instance()->postProcess();
    player->onRender();
}

CharacterManager::CharacterManager()
{
    player = new Player();
    enemy = new Enemy();

    player->setHp(10);
    enemy->setHp(25);
}

CharacterManager::~CharacterManager()
{
    delete enemy;
    delete player;
}