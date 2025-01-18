#include "enemy.h"
#include "characterManager.h"
#include "enemyStateNodes.h"

// Aim
EnemyAimState::EnemyAimState()
{
    timer.setOneShot(true);
    timer.setWaitTime(0.5f);
    timer.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            enemy->setGravityEnabled(true);
            enemy->switchState("dashInAir");
        });
}

void EnemyAimState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("aim");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setGravityEnabled(false);
    enemy->setVelocity({ 0, 0 });
    timer.restart();
}

void EnemyAimState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timer.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
}

// dashInAir
void EnemyDashInAirState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("dashInAir");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    const Character* player = CharacterManager::instance()->getPlayer();
    Vector2 posTarget = { player->getPosition().x, player->getFloorY() };
    enemy->setVelocity((posTarget - enemy->getPosition()).normalize() * SPEED_DASH);
    enemy->setDashingInAir(true);
    enemy->setGravityEnabled(false);
    enemy->onDash();

    playAudio(_T("enemy_dash"), false);
}

void EnemyDashInAirState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
    else if (enemy->isOnFloor())
        enemy->switchState("idle");
}

void EnemyDashInAirState::onExit()
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    enemy->setGravityEnabled(true);
    enemy->setDashingInAir(false);
}

// dashOnFloor
EnemyDashOnFloorState::EnemyDashOnFloorState()
{
    timer.setOneShot(true);
    timer.setWaitTime(0.5f);
    timer.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            enemy->setDashingOnFloor(false);
        });
}

void EnemyDashOnFloorState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("dashOnFloor");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setVelocity({ enemy->getFacingLeft() ? -SPEED_DASH : SPEED_DASH, 0 });
    enemy->setDashingOnFloor(true);
    enemy->onDash();
    timer.restart();

    playAudio(_T("enemy_dash"), false);
}

void EnemyDashOnFloorState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timer.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
    else if (!enemy->getDashingOnFloor())
        enemy->switchState("idle");
}

// dead
void EnemyDeadState::onEnter()
{
    MessageBox(GetHWnd(), _T("good!"), _T("Game victory! "), MB_OK);
    exit(0);
}

// fall
void EnemyFallState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("fall");
}

void EnemyFallState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
    else if (enemy->isOnFloor())
        enemy->switchState("idle");
}

// idle
EnemyIdleState::EnemyIdleState()
{
    timer.setOneShot(true);
    timer.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            
            int randNum = rangeRandom(0, 100);
            if (enemy->getHp() > 20)
            {
                if (randNum <= 25)
                {
                    if (!enemy->isOnFloor())
                        enemy->switchState("fall");
                    else
                        enemy->switchState("jump");     // 25%
                }
                else if (randNum <= 50)
                {
                    if (!enemy->isOnFloor())
                        enemy->switchState("fall");
                    else
                        enemy->switchState("run");      // 25%
                }
                else if (randNum <= 80)
                    enemy->switchState("squat");        // 30%
                else if (randNum <= 90)
                    enemy->switchState("throwSilk");    // 10%
                else
                    enemy->switchState("throwSword");   // 10%
            }
            else
            {
                if (randNum <= 25)
                {
                    if (!enemy->isOnFloor())
                        enemy->switchState("fall");
                    else
                        enemy->switchState("jump");     // 25%
                }
                else if (randNum <= 60)
                    enemy->switchState("throwSword");   // 35%
                else if (randNum <= 70)
                    enemy->switchState("throwSilk");    // 10%
                else if (randNum <= 90)
                    enemy->switchState("throwBarb");    // 20%
                else
                    enemy->switchState("squat");        // 10%
            }
        });
}

void EnemyIdleState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("idle");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setVelocity({ 0, 0 });

    float waitTime = 0;
    if (enemy->getHp() > 5)
        waitTime = rangeRandom(0, 2) * 0.25f;  // 0.0s ~ 0.5s
    else
        waitTime = rangeRandom(0, 1) * 0.25f;  // 0.0s ~ 0.25s
    
    timer.setWaitTime(waitTime);
    timer.restart();
}

void EnemyIdleState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timer.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
    else if (enemy->getVelocity().y > 0)
        enemy->switchState("fall");
}

void EnemyIdleState::onExit()
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    enemy->setFacingLeft(enemy->getPosition().x > CharacterManager::instance()->getPlayer()->getPosition().x);
}

// jump
void EnemyJumpState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("jump");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setVelocity({ 0, -SPEED_JUMP });
}

void EnemyJumpState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
    else if (enemy->getVelocity().y > 0)
    {
        int randNum = rangeRandom(0, 100);
        if (enemy->getHp() > 10)
        {
            if (randNum <= 50)
                enemy->switchState("aim");          // 50%
            else if (randNum <= 80)
                enemy->switchState("fall");         // 30%
            else
                enemy->switchState("throwSilk");    // 20%
        }
        else
        {
            if (randNum <= 50)
                enemy->switchState("throwSilk");    // 50
            else if (randNum <= 80)
                enemy->switchState("fall");         // 30%
            else
                enemy->switchState("aim");          // 20%
        }
    }
}

// run
void EnemyRunState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("run");

    playAudio(_T("enemy_run"), true);
}

void EnemyRunState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    const Vector2& posEnemy = enemy->getPosition();
    const Vector2& posPlayer = CharacterManager::instance()->getPlayer()->getPosition();
    enemy->setVelocity({ posEnemy.x < posPlayer.x ? SPEED_RUN : -SPEED_RUN, 0 });

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
    else if (abs(posEnemy.x - posPlayer.x) <= MIN_DIS)
    {
        int randNum = rangeRandom(0, 100);
        if (enemy->getHp() > 10)
        {
            if (randNum <= 75)
                enemy->switchState("squat");        // 75%
            else 
                enemy->switchState("throwSilk");    // 25%
        }
        else
        {
            if (randNum <= 75)
                enemy->switchState("throwSilk");    // 75%
            else
                enemy->switchState("squat");        // 25%
        }
        stopAudio("enemy_run");
    }
}

void EnemyRunState::onExit()
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setVelocity({ 0, 0 });
}

// squat
EnemySquatState::EnemySquatState()
{
    timer.setOneShot(true);
    timer.setWaitTime(0.5f);
    timer.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            enemy->switchState("dashOnFloor");
        });
}

void EnemySquatState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("squat");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setFacingLeft(enemy->getPosition().x > CharacterManager::instance()->getPlayer()->getPosition().x);
    timer.restart();
}

void EnemySquatState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timer.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
}

// throw barb
EnemyThrowBarbState::EnemyThrowBarbState()
{
    timer.setOneShot(true);
    timer.setWaitTime(0.8f);
    timer.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            enemy->throwBarbs();
            enemy->switchState("idle");
        });
}

void EnemyThrowBarbState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("throwBarb");

    timer.restart();

    playAudio(_T("enemy_throw_barbs"), false);
}

void EnemyThrowBarbState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timer.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
}

// throw silk
EnemyThrowSilkState::EnemyThrowSilkState()
{
    timer.setOneShot(true);
    timer.setWaitTime(0.9f);
    timer.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            enemy->setGravityEnabled(true);
            enemy->setThrowingSilk(false);
            if (!enemy->isOnFloor() && enemy->getHp() > 10 && rangeRandom(0, 100) <= 25)
                enemy->switchState("aim");
            else if (!enemy->isOnFloor())
                enemy->switchState("fall");
            else
                enemy->switchState("idle");
        });
}

void EnemyThrowSilkState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("throwSilk");

    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
    enemy->setGravityEnabled(false);
    enemy->setThrowingSilk(true);
    enemy->setVelocity({ 0, 0 });
    enemy->onThrowSilk();
    timer.restart();

    playAudio(_T("enemy_throw_silk"), false);  
}

void EnemyThrowSilkState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timer.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
}

// throw sword
EnemyThrowSwordState::EnemyThrowSwordState()
{
    timerThrow.setWaitTime(0.65f);
    timerThrow.setOneShot(true);
    timerThrow.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();
            enemy->throwSword();

            playAudio(_T("enemy_throw_sword"), false);
        });
    
    timerSwitch.setWaitTime(1.0f);
    timerSwitch.setOneShot(true);
    timerSwitch.setOnTimeout([&]()
        {
            Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

            int randNum = rangeRandom(0, 100);
            if (enemy->getHp() > 20)
            {
                if (randNum <= 50)
                    enemy->switchState("squat");        // 50%
                else if (randNum <= 80)
                    enemy->switchState("jump");         // 30%
                else
                    enemy->switchState("idle");         // 20%
            }
            else
            {
                if (randNum <= 50)
                    enemy->switchState("jump");         // 50%
                else if (randNum <= 80)
                    enemy->switchState("throwSilk");    // 30%
                else
                    enemy->switchState("idle");         // 20%
            }
        });
}

void EnemyThrowSwordState::onEnter()
{
    CharacterManager::instance()->getEnemy()->setAnimation("throwSword");
    timerThrow.restart();
    timerSwitch.restart();
}

void EnemyThrowSwordState::onUpdate(float delta)
{
    Enemy* enemy = (Enemy*)CharacterManager::instance()->getEnemy();

    timerThrow.onUpdate(delta);
    timerSwitch.onUpdate(delta);

    if (enemy->getHp() <= 0)
        enemy->switchState("dead");
}