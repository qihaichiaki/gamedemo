#ifndef _PLAYER_STATE_NODES_H_
#define _PLAYER_STATE_NODES_H_

#include "timer.h"
#include "stateNode.h"

class PlayerAttackState: public StateNode
{
public:
    PlayerAttackState();
    ~PlayerAttackState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;

private:
    Timer timer;

private:
    void updateHitBoxPosition();
};

class PlayerDeadState: public StateNode
{
public:
    PlayerDeadState();
    ~PlayerDeadState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;

private:
    Timer timer;
};

class PlayerFallState: public StateNode
{
public:
    PlayerFallState();
    ~PlayerFallState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
};

class PlayerIdleState: public StateNode
{
public:
    PlayerIdleState();
    ~PlayerIdleState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
};

class PlayerJumpState: public StateNode
{
public:
    PlayerJumpState();
    ~PlayerJumpState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
};

class PlayerRollState: public StateNode
{
public:
    PlayerRollState();
    ~PlayerRollState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;

private:
    Timer timer;
};

class PlayerRunState: public StateNode
{
public:
    PlayerRunState();
    ~PlayerRunState() = default;

    void onEnter() override;
    void onUpdate(float delta) override;
    void onExit() override;
};

#endif