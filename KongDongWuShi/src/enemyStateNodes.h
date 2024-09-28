#ifndef _ENEMY_STATE_NODES_H_
#define _ENEMY_STATE_NODES_H_

#include "timer.h"
#include "stateNode.h"

class EnemyAimState: public StateNode
{
public:
    EnemyAimState();
    ~EnemyAimState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    Timer timer;
};

class EnemyDashInAirState: public StateNode
{
public:
    EnemyDashInAirState() = default;
    ~EnemyDashInAirState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;

private:
    const float SPEED_DASH = 1500.0f;
};

class EnemyDashOnFloorState: public StateNode
{
public:
    EnemyDashOnFloorState();
    ~EnemyDashOnFloorState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    const float SPEED_DASH = 1000.0f;

private:
    Timer timer;
};

class EnemyDeadState: public StateNode
{
public:
    EnemyDeadState() = default;
    ~EnemyDeadState() = default;

    void onEnter() override;
};

class EnemyFallState: public StateNode
{
public:
    EnemyFallState() = default;
    ~EnemyFallState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
};

class EnemyIdleState: public StateNode
{
public:
    EnemyIdleState();
    ~EnemyIdleState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;

private:
    Timer timer;
};

class EnemyJumpState: public StateNode
{
public:
    EnemyJumpState() = default;
    ~EnemyJumpState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    const float SPEED_JUMP = 800.0f;
};

class EnemyRunState: public StateNode
{
public:
    EnemyRunState() = default;
    ~EnemyRunState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;

private:
    const float MIN_DIS = 350.0f;
    const float SPEED_RUN = 500.0f;
};


class EnemySquatState: public StateNode
{
public:
    EnemySquatState();
    ~EnemySquatState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    Timer timer;
};

class EnemyThrowBarbState: public StateNode
{
public:
    EnemyThrowBarbState();
    ~EnemyThrowBarbState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    Timer timer;
};

class EnemyThrowSilkState: public StateNode
{
public:
    EnemyThrowSilkState();
    ~EnemyThrowSilkState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    Timer timer;
};

class EnemyThrowSwordState: public StateNode
{
public:
    EnemyThrowSwordState();
    ~EnemyThrowSwordState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    Timer timerThrow;
    Timer timerSwitch;
};

#endif