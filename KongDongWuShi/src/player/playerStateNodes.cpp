#include "player.h"
#include "characterManager.h"
#include "playerStateNodes.h"

// PlayerAttackState

PlayerAttackState::PlayerAttackState()
{
    timer.setWaitTime(0.3f);
    timer.setOneShot(true);
    timer.setOnTimeout([&]()
        {
            Player* player = reinterpret_cast<Player*>(CharacterManager::instance()->getPlayer());
            player->setAttacking(false);
        });
}

void PlayerAttackState::onEnter()
{
    Player* player = (Player*)(CharacterManager::instance()->getPlayer());
    player->setAnimation("attack");

    player->getHitBox()->setEnabled(true);
    player->setAttacking(true);
    updateHitBoxPosition();
    player->onAttack();
    timer.restart();

    switch (rangeRandom(1, 3))
    {
    case 1:
        playAudio(_T("player_attack_1"), false);
        break;
    case 2:
        playAudio(_T("player_attack_2"), false);
        break;
    case 3:
        playAudio(_T("player_attack_3"), false);
        break;    
    default:
        break;
    }
}

void PlayerAttackState::onUpdate(float delta)
{
    timer.onUpdate(delta);
    updateHitBoxPosition();

    Player* player = (Player*)CharacterManager::instance()->getPlayer();

    if (player->getHp() <= 0)
        player->switchState("dead");
    else if (!player->getAttacking())
    {
        if (player->getVelocity().y > 0)
            player->switchState("fall");
        else if (player->getMoveAxis() == 0)
            player->switchState("idle");
        else if (player->isOnFloor() && player->getMoveAxis() != 0)
            player->switchState("run");
    }
}

void PlayerAttackState::onExit()
{
    Player* player = (Player*)CharacterManager::instance()->getPlayer();
    player->getHitBox()->setEnabled(false);
    player->setAttacking(false);
}

void PlayerAttackState::updateHitBoxPosition()
{
    Player* player = (Player*)CharacterManager::instance()->getPlayer();
    Vector2 posCenter = player->getLogicCenter();
    CollisionBox* hitBox = player->getHitBox();
    const Vector2& sizeHitBox = hitBox->getSize();
    Vector2 posHitBox;
    switch (player->getAttackDir())
    {
    case Player::AttackDir::Up:
        posHitBox = { posCenter.x, posCenter.y - sizeHitBox.y / 2 };
        break;
    case Player::AttackDir::Down:
        posHitBox = { posCenter.x, posCenter.y + sizeHitBox.y / 2 };
        break;
    case Player::AttackDir::Left:
        posHitBox = { posCenter.x - sizeHitBox.y / 2, posCenter.y };
        break;
    case Player::AttackDir::Right:
        posHitBox = { posCenter.x + sizeHitBox.y / 2, posCenter.y };
        break;
    default:
        break;
    }
    hitBox->setPosition(posHitBox);
}

// PlayerDeadState

PlayerDeadState::PlayerDeadState()
{
    timer.setWaitTime(2.0f);
    timer.setOneShot(true);
    timer.setOnTimeout([&]()
        {
            MessageBox(GetHWnd(), _T("I'm sorry. I can't die yet......"), _T("Game over!"), MB_OK);
            exit(0);
        });
}

void PlayerDeadState::onEnter()
{
    CharacterManager::instance()->getPlayer()->setAnimation("dead");

    playAudio(_T("player_dead"), false);
}

void PlayerDeadState::onUpdate(float delta)
{
    timer.onUpdate(delta);
}

// PlayerFallState

PlayerFallState::PlayerFallState() = default;

void PlayerFallState::onEnter()
{
    CharacterManager::instance()->getPlayer()->setAnimation("fall");
}

void PlayerFallState::onUpdate(float delta)
{
    Player* player = (Player*)CharacterManager::instance()->getPlayer();
    
    if (player->getHp() <= 0)
        player->switchState("dead");
    else if (player->isOnFloor())
    {
        player->switchState("idle");
        player->onLand();

        playAudio(_T("player_land"), false);
    }
    else if (player->canAttack())
        player->switchState("attack");
}

// PlayerIdleState

PlayerIdleState::PlayerIdleState() = default;

void PlayerIdleState::onEnter()
{
    CharacterManager::instance()->getPlayer()->setAnimation("idle");
}

void PlayerIdleState::onUpdate(float delta)
{
    Player* player = (Player*)CharacterManager::instance()->getPlayer();

    if (player->getHp() <= 0)
        player->switchState("dead");
    else if (player->canAttack())
        player->switchState("attack");
    else if (player->getVelocity().y > 0)
        player->switchState("fall");
    else if (player->canJump())
        player->switchState("jump");
    else if (player->canRoll())
        player->switchState("roll");
    else if (player->isOnFloor() && player->getMoveAxis() != 0)
        player->switchState("run");
}

// PlayerJumpState

PlayerJumpState::PlayerJumpState() = default;

void PlayerJumpState::onEnter()
{
    CharacterManager::instance()->getPlayer()->setAnimation("jump");

    Player* player = (Player*)CharacterManager::instance()->getPlayer();
    player->onJump();

    playAudio(_T("player_jump"), false);
}

void PlayerJumpState::onUpdate(float delta)
{
    Player* player = (Player*)CharacterManager::instance()->getPlayer();

    if (player->getHp() <= 0)
        player->switchState("dead");
    else if (player->getVelocity().y > 0)
        player->switchState("fall");
    else if (player->canAttack())
        player->switchState("attack");
}

// PlayerRollState

PlayerRollState::PlayerRollState()
{
    timer.setWaitTime(0.35f);
    timer.setOneShot(true);
    timer.setOnTimeout([&]()
        {
            Player* player = (Player*)CharacterManager::instance()->getPlayer();
            player->setRolling(false);
        });
}

void PlayerRollState::onEnter()
{
    CharacterManager::instance()->getPlayer()->setAnimation("roll");

    Player* player = (Player*)CharacterManager::instance()->getPlayer();
    player->getHurtBox()->setEnabled(false);
    player->setRolling(true);
    player->onRoll();
    timer.restart();

    playAudio(_T("player_roll"), false);
}

void PlayerRollState::onUpdate(float delta)
{
    timer.onUpdate(delta);

    Player* player = (Player*)CharacterManager::instance()->getPlayer();

    if (!player->getRolling())
    {
        if (player->getMoveAxis() != 0)
            player->switchState("run");
        else if (player->canJump())
            player->switchState("jump");
        else
            player->switchState("idle");
    }
}

void PlayerRollState::onExit()
{
    CharacterManager::instance()->getPlayer()->getHurtBox()->setEnabled(true);
}

// PlayerRunState

PlayerRunState::PlayerRunState() = default;

void PlayerRunState::onEnter()
{
    CharacterManager::instance()->getPlayer()->setAnimation("run");

    playAudio(_T("player_run"), true);
}

void PlayerRunState::onUpdate(float delta)
{
    Player* player = (Player*)CharacterManager::instance()->getPlayer();

    if (player->getHp() <= 0)
        player->switchState("dead");
    else if (player->getMoveAxis() == 0)
        player->switchState("idle");
    else if (player->canJump())
        player->switchState("jump");
    else if (player->canAttack())
        player->switchState("attack");
    else if (player->canRoll())
        player->switchState("roll");
}

void PlayerRunState::onExit()
{
    stopAudio(_T("player_run"));
}