#include "enemy.h"
#include "characterManager.h"
#include "resourcesManager.h"
#include "collisionManager.h"
#include "enemyStateNodes.h"

Enemy::Enemy()
{
    isFacingLeft = true;
    position = { 1050, 200 };
    logicHeight = 150;

    hitBox->setSize({ 50, 80 });
    hurtBox->setSize({ 100, 180 });

    hitBox->setLayerSrc(CollisionLayer::None);
    hitBox->setLayerDst(CollisionLayer::Player);

    hurtBox->setLayerSrc(CollisionLayer::Enemy);
    hurtBox->setLayerDst(CollisionLayer::Player);
    hurtBox->setOnCollide([&]()
        {
            decreaseHp();
        });

    // silk box
    collisionBoxSilk = CollisionManager::instance()->createCollisionBox();
    collisionBoxSilk->setLayerSrc(CollisionLayer::None);
    collisionBoxSilk->setLayerDst(CollisionLayer::Player);
    collisionBoxSilk->setSize({ 225, 225 });
    collisionBoxSilk->setEnabled(false);

    // 动画初始化部分
    {
        // aim
        {
            AnimationGroup& animationGroup = animationPool["aim"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_aim_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_aim_right"));
        }

        // dash_in_air
        {
            AnimationGroup& animationGroup = animationPool["dashInAir"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_dash_in_air_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_dash_in_air_right"));
        }

        // dash_on_floor
        {
            AnimationGroup& animationGroup = animationPool["dashOnFloor"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_dash_on_floor_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_dash_on_floor_right"));
        }
    
        // fall
        {
            AnimationGroup& animationGroup = animationPool["fall"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_fall_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_fall_right"));
        }

        // idle
        {
            AnimationGroup& animationGroup = animationPool["idle"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_idle_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_idle_right"));
        }

        // jump
        {
            AnimationGroup& animationGroup = animationPool["jump"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_jump_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_jump_right"));
        }

        // run
        {
            AnimationGroup& animationGroup = animationPool["run"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_run_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_run_right"));
        }

        // squat
        {
            AnimationGroup& animationGroup = animationPool["squat"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_squat_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_squat_right"));
        }

        // throw_barb
        {
            AnimationGroup& animationGroup = animationPool["throwBarb"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_throw_barb_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_throw_barb_right")); 
        }

        // throw_silk
        {
            AnimationGroup& animationGroup = animationPool["throwSilk"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_throw_silk_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_throw_silk_right"));
        }

        // throw_sword
        {
            AnimationGroup& animationGroup = animationPool["throwSword"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_throw_sword_left"));

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_throw_sword_right"));
        }
    }

    {
        animationSilk.setInterval(0.1f);
        animationSilk.setLoop(false);
        animationSilk.setAnchorMode(Animation::AnchorMode::Centered);
        animationSilk.addFrame(ResourcesManager::instance()->findAtlas("silk"));

        Animation& animationDashInAirLeft = animationDashInAirVfx.left;
        animationDashInAirLeft.setInterval(0.1f);
        animationDashInAirLeft.setLoop(false);
        animationDashInAirLeft.setAnchorMode(Animation::AnchorMode::Centered);
        animationDashInAirLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_vfx_dash_in_air_left"));
    
        Animation& animationDashInAirRight = animationDashInAirVfx.right;
        animationDashInAirRight.setInterval(0.1f);
        animationDashInAirRight.setLoop(false);
        animationDashInAirRight.setAnchorMode(Animation::AnchorMode::Centered);
        animationDashInAirRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_vfx_dash_in_air_right"));


        Animation& animationDashOnFloorLeft = animationDashOnFloorVfx.left;
        animationDashOnFloorLeft.setInterval(0.1f);
        animationDashOnFloorLeft.setLoop(false);
        animationDashOnFloorLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
        animationDashOnFloorLeft.addFrame(ResourcesManager::instance()->findAtlas("enemy_vfx_dash_on_floor_left"));
    
        Animation& animationDashOnFloorRight = animationDashOnFloorVfx.right;
        animationDashOnFloorRight.setInterval(0.1f);
        animationDashOnFloorRight.setLoop(false);
        animationDashOnFloorRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
        animationDashOnFloorRight.addFrame(ResourcesManager::instance()->findAtlas("enemy_vfx_dash_on_floor_right"));
    }

    {
        stateMachine.registerState("aim", new EnemyAimState());
        stateMachine.registerState("dashInAir", new EnemyDashInAirState());
        stateMachine.registerState("dashOnFloor", new EnemyDashOnFloorState());
        stateMachine.registerState("dead", new EnemyDeadState());
        stateMachine.registerState("fall", new EnemyFallState());
        stateMachine.registerState("idle", new EnemyIdleState());
        stateMachine.registerState("jump", new EnemyJumpState());
        stateMachine.registerState("run", new EnemyRunState());
        stateMachine.registerState("squat", new EnemySquatState());
        stateMachine.registerState("throwBarb", new EnemyThrowBarbState());
        stateMachine.registerState("throwSilk", new EnemyThrowSilkState());
        stateMachine.registerState("throwSword", new EnemyThrowSwordState());

        stateMachine.setEntry("idle");
    }
}

Enemy::~Enemy()
{
    CollisionManager::instance()->destroyCollisionBox(collisionBoxSilk);
}

void Enemy::onUpdate(float delta)
{
    if (velocity.x >= 0.0001f)
        isFacingLeft = (velocity.x < 0);
    
    Character::onUpdate(delta);

    hitBox->setPosition(getLogicCenter());

    if (isThrowingSilk)
    {
        collisionBoxSilk->setPosition(getLogicCenter());
        animationSilk.setPosition(getLogicCenter());
        animationSilk.onUpdate(delta);
    }

    if (isDashingInAir || isDashingOnFloor)
    {
        currentDashAnimation->setPosition(isDashingInAir ? getLogicCenter() : position);
        currentDashAnimation->onUpdate(delta);
    }

    for (Barb* barb : barbList)
        barb->onUpdate(delta);
    for (Sword* sword : swordList)
        sword->onUpdate(delta);
    
    // 利用 erase + remove_if 移除掉无用的东西
    barbList.erase(std::remove_if(
        barbList.begin(), barbList.end(),
        [](Barb* barb)
        {
            bool canRemove = !barb->checkValid();
            if (canRemove) delete barb;
            return canRemove;
        }),
        barbList.end());
    swordList.erase(std::remove_if(
        swordList.begin(), swordList.end(),
        [](Sword* sword)
        {
            bool canRemove = !sword->checkValid();
            if (canRemove) delete sword;
            return canRemove;
        }),
        swordList.end());
}

void Enemy::onRender()
{
    for (Barb* barb : barbList)
        barb->onRender();
    for (Sword* sword : swordList)
        sword->onRender();
    
    Character::onRender();

    if (isThrowingSilk)
        animationSilk.onRender();
    
    if (isDashingInAir || isDashingOnFloor)
        currentDashAnimation->onRender();
}

void Enemy::onHurt()
{
    switch (rangeRandom(1, 3))
    {
    case 1:
        playAudio(_T("enemy_hurt_1"), false);
        break;
    case 2:
        playAudio(_T("enemy_hurt_2"), false);
        break;
    case 3:
        playAudio(_T("enemy_hurt_3"), false);
        break;    
    }
}

// 召唤刺球
void Enemy::throwBarbs()
{
    int numNewBarb = rangeRandom(3, 6);
    if (barbList.size() >= 10) numNewBarb = 1;
    int widthGrid = getwidth() / numNewBarb;

    for (int i = 0; i < numNewBarb; ++i)
    {
        Barb* newBarb = new Barb();
        int randX = rangeRandom(widthGrid * i, widthGrid * (i + 1));
        int randY = rangeRandom(250, 500);
        newBarb->setPosition({ (float)randX, (float)randY });
        barbList.push_back(newBarb);
    }
}

// 扔剑
void Enemy::throwSword()
{
    Sword* newSword = new Sword(getLogicCenter(), isFacingLeft);
    swordList.push_back(newSword);
}

// 冲刺
void Enemy::onDash()
{
    if (isDashingInAir)
        currentDashAnimation = velocity.x < 0 ? &animationDashInAirVfx.left : &animationDashInAirVfx.right;
    else
        currentDashAnimation = velocity.x < 0 ? &animationDashOnFloorVfx.left : &animationDashOnFloorVfx.right;
    
    currentDashAnimation->reset();
}

// 缠绕丝线
void Enemy::onThrowSilk()
{
    animationSilk.reset();
}