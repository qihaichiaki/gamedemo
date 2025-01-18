#ifndef _BARB_H_
#define _BARB_H_
// 刺球对象

#include "animation.h"
#include "collisionBox.h"

class Barb
{
public:
    Barb();
    ~Barb();

    void onUpdate(float delta);
    void onRender();

    void setPosition(const Vector2& position)
    {
        this->basePosition = position;
        this->currentPosition = position;
    }

    bool checkValid() const { return isValid; }

private:
    // TODO: 这里可以使用状态机进行控制
    enum class Stage
    {
        Idle,  // 上下浮动状态
        Aim,   // 瞄准状态
        Dash,  // 冲刺状态
        Break  // 破碎状态
    };

private:
    const float SPEED_DASH = 1500.0f;

private:
    Timer timerIdle;
    Timer timerAim;
    int diffPeriod = 0;  // 随机值，控制浮动的运动周期偏移
    bool isValid = true;
    float totalDeltaTime = 0;  // 存在时间

    Vector2 velocity;
    Vector2 basePosition;
    Vector2 currentPosition;

    Animation animationLoose;
    Animation animationBreak;
    Animation* currentAnimation = nullptr;

    Stage stage = Stage::Idle;
    CollisionBox* collisionBox = nullptr;

private:
    void onBreak();  // 破碎时的处理
};

#endif