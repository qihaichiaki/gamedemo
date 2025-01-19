#ifndef _CHARACTER_MANAGER_H_
#define _CHARACTER_MANAGER_H_

#include "character.h"

class CharacterManager
{
public:
    static CharacterManager* instance();

    Character* getEnemy()
    {
        return enemy;
    }

    Character* getPlayer()
    {
        return player;
    }

    void onInput(const ExMessage& msg);
    void onUpdate(float delta);
    void onRender();

private:
    Character* enemy = nullptr;
    Character* player = nullptr;

private:
    CharacterManager();
    CharacterManager(const CharacterManager&);
    ~CharacterManager();
};

#endif