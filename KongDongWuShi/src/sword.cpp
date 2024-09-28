#include "sword.h"
#include "resourcesManager.h"
#include "collisionManager.h"

Sword::Sword(const Vector2& position, bool moveLeft)
{
    // 动画属性
    animation.setInterval(0.1f);
    animation.setLoop(true);
    animation.setAnchorMode(Animation::AnchorMode::Centered);
    animation.addFrame(ResourcesManager::instance()->findAtlas(
        moveLeft ? "sword_left" : "sword_right"
    ));

    // 碰撞盒子
    collisionBox = CollisionManager::instance()->createCollisionBox();
    collisionBox->setLayerSrc(CollisionLayer::None);
    collisionBox->setLayerDst(CollisionLayer::Player);
    collisionBox->setSize({ 195, 10 });

    this->position = position;
    this->velocity = { moveLeft ? -SPEED_MOVE : SPEED_MOVE, 0 }; 
}

Sword::~Sword()
{
    CollisionManager::instance()->destroyCollisionBox(collisionBox);
}

void Sword::onUpdate(float delta)
{
    position += velocity * delta;
    animation.setPosition(position);
    collisionBox->setPosition(position);

    animation.onUpdate(delta);

    if (position.x <= -200 || position.x >= getwidth() + 200)
        isValid = false;
}

void Sword::onRender()
{
    animation.onRender();
}