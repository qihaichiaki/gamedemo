#ifndef _SWORD_H_
#define _SWORD_H_

#include "animation.h"
#include "collisionBox.h"

class Sword
{
public:
    Sword(const Vector2& position, bool moveLeft);
    ~Sword();

    void onUpdate(float delta);
    void onRender();

    bool checkValid() const { return isValid; }

private:
    const float SPEED_MOVE = 1250.0f;

private:
    Vector2 position;
    Vector2 velocity;
    Animation animation;
    bool isValid = true;
    CollisionBox* collisionBox = nullptr;
};

#endif