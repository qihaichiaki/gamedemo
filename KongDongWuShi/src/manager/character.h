#ifndef _CHARACTER_H_
#define _CHARACTER_H_

// 角色基类

#include "vector2.h"
#include "animation.h"
#include "collisionBox.h"
#include "stateMachine.h"

#include <string>
#include <graphics.h>
#include <unordered_map>

class Character
{
public:
    Character();
    ~Character();

    void decreaseHp();

    int getHp() const { return hp; }
    void setHp(int hp) { this->hp = hp; }

    void setPosition(const Vector2& position) { this->position = position; }

    const Vector2& getPosition() const { return position; }

    void setVelocity(const Vector2& velocity) { this->velocity = velocity; }

    const Vector2& getVelocity() { return velocity; }

    Vector2 getLogicCenter() const
    {
        return Vector2(position.x, position.y - logicHeight / 2);
    }

    void setGravityEnabled(bool flag) { enableGravity = flag; }

    CollisionBox* getHitBox() { return hitBox; }

    CollisionBox* getHurtBox() { return hurtBox; }

    bool isOnFloor() const { return position.y >= FLOOR_Y; }

    float getFloorY() const { return FLOOR_Y; }

    void makeInvulnerable()
    {
        isInvulnerable = true;
        timerInvulnerableStatus.restart();
    }

    virtual void onInput(const ExMessage& msg);
    virtual void onUpdate(float delta);
    virtual void onRender();

    virtual void onHurt();

    void switchState(const std::string& id);
    void setAnimation(const std::string& id);

protected:
    struct AnimationGroup
    {
        Animation left;
        Animation right;
    };
protected:
    const float FLOOR_Y = 620;                                      // 地板的竖直方向坐标
    const float GRAVITY = 980;                                      // 重力大小

protected:
    int hp = 10;                                                    // 角色生命值
    Vector2 position;                                               // 角色脚底位置
    Vector2 velocity;                                               // 角色速度
    float logicHeight = 0;                                          // 角色的逻辑高度
    bool isFacingLeft = true;                                       // 当前角色的朝向是否左
    StateMachine stateMachine;                                      // 角色逻辑状态机
    bool enableGravity = true;                                      // 启用重力模拟
    bool isInvulnerable = false;                                    // 当前是否无敌
    Timer timerInvulnerableBlink;                                   // 无敌闪烁状态定时器
    Timer timerInvulnerableStatus;                                  // 无敌状态定时器
    bool isBlinkInvisible = false;                                  // 当前是否处于闪烁的不可见帧
    CollisionBox* hitBox = nullptr;                                 // 攻击碰撞箱
    CollisionBox* hurtBox = nullptr;                                // 受击碰撞箱
    AnimationGroup* currentAnimation = nullptr;                     // 当前角色动画
    std::unordered_map<std::string, AnimationGroup> animationPool;  // 角色动画池
};


#endif