#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "character.h"

class Player: public Character
{
public:
    enum class AttackDir
    {
        Up, Down, Left, Right
    };

public:
    Player();
    ~Player();

    void onInput(const ExMessage& msg) override;
    void onUpdate(float delta) override;
    void onRender() override;

    void onHurt() override;

    // roll
    void setRolling(bool flag) { isRolling = flag; }
    bool getRolling() const { return isRolling; }
    bool canRoll() const { return isRollCdComp && !isRolling && isRollKeyDown; }

    // attack
    void setAttacking(bool flag) { isAttacking = flag; }
    bool getAttacking() const { return isAttacking; }
    bool canAttack() const { return isAttackCdComp && !isAttacking && isAttackKeyDown; }

    bool canJump() const { return isOnFloor() && isJumpKeyDown; }

    int getMoveAxis() const { return static_cast<int>(isRightKeyDown - isLeftKeyDown); };

    AttackDir getAttackDir() const { return attackDir; };

    // 具体执行的代码逻辑
    void onJump();
    void onLand();
    void onRoll();
    void onAttack();

private:
    // 翻滚状态标志
    // 翻滚冷却
    Timer timerRollCd;
    bool isRolling = false;
    bool isRollCdComp = true;

    // 攻击标志
    Timer timerAttackCd;
    bool isAttacking = false;
    bool isAttackCdComp = true;

    // 键位到逻辑的映射状态
    bool isLeftKeyDown = false;
    bool isRightKeyDown = false;
    bool isJumpKeyDown = false;
    bool isRollKeyDown = false;
    bool isAttackKeyDown = false;

    // 攻击动画特效
    Animation animationSlashUp;
    Animation animationSlashDown;
    Animation animationSlashLeft;
    Animation animationSlashRight;
    AttackDir attackDir = AttackDir::Right;
    Animation* currentSlashAnimation = nullptr;

    // 上跳落下 烟雾特效
    bool isJumpVfxVisible = false;
    Animation animationJumpVfx;

    bool isLandVfxVisible = false;
    Animation animationLandVfx;

private:
    const float CD_ROLL = 0.075f;
    const float CD_ATTACK = 0.5f;
    const float SPEED_RUN = 300.0f;
    const float SPEED_JUMP = 780.0f;
    const float SPEED_ROLL = 800.0f;

private:
    // 传入鼠标点击的坐标，计算转换为攻击的方向
    void updateAttackDir(int x, int y);
};

#endif