#include "character.h"
#include "collisionManager.h"

#include <iostream>

Character::Character()
{
    hitBox = CollisionManager::instance()->createCollisionBox();
    hurtBox = CollisionManager::instance()->createCollisionBox();

    timerInvulnerableStatus.setWaitTime(1.0f);
    timerInvulnerableStatus.setOneShot(true);
    timerInvulnerableStatus.setOnTimeout([&]()
        {
            isInvulnerable = false;
        });
    
    timerInvulnerableBlink.setWaitTime(0.075f);
    timerInvulnerableBlink.setOneShot(false);
    timerInvulnerableBlink.setOnTimeout([&]()
        {
            isBlinkInvisible = !isBlinkInvisible;
        });
}

Character::~Character()
{
    CollisionManager::instance()->destroyCollisionBox(hitBox);
    CollisionManager::instance()->destroyCollisionBox(hurtBox);
}

void Character::decreaseHp()
{
    if (isInvulnerable) return;

    hp -= 1;
    if (hp > 0)
        makeInvulnerable();
    onHurt();
}

void Character::onInput(const ExMessage& msg) { }

void Character::onUpdate(float delta)
{
    // 状态机的更新
    stateMachine.onUpdate(delta);

    if (hp <= 0)
        velocity.x = 0;
    if (enableGravity)
        velocity.y += GRAVITY * delta;  // 模拟重力
    
    // 坐标位置
    position += velocity * delta;

    // 在地板下方修正位置
    if (position.y >= FLOOR_Y)
    {
        position.y = FLOOR_Y;
        velocity.y = 0;
    }

    // 防止角色出水平屏幕外
    if (position.x <= 0) position.x = 0;
    if (position.x >= getwidth()) position.x = static_cast<float>(getwidth());

    // 设置受击碰撞箱一定是和角色移动的
    hurtBox->setPosition(getLogicCenter());

    timerInvulnerableStatus.onUpdate(delta);

    if (isInvulnerable)
        timerInvulnerableBlink.onUpdate(delta);
    
    if (!currentAnimation) return;

    // 设置方向对应不同的动画，执行更新和位置设置
    Animation& animation = (isFacingLeft
        ? currentAnimation->left : currentAnimation->right);
    animation.onUpdate(delta);
    animation.setPosition(position);
}

void Character::onRender()
{
    // 当前动画没有设置或者处于无敌状态中不可见的帧时
    if (!currentAnimation || (isInvulnerable && isBlinkInvisible)) return;

    (isFacingLeft ? currentAnimation->left : currentAnimation->right).onRender();
}

void Character::onHurt() { }

void Character::switchState(const std::string& id)
{
    stateMachine.switchTo(id);
}

void Character::setAnimation(const std::string& id)
{
    currentAnimation = &animationPool[id];

    currentAnimation->left.reset();
    currentAnimation->right.reset();
}