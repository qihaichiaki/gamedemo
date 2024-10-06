#include "player.h"
#include "resourcesManager.h"
#include "playerStateNodes.h"
#include "bulletTimeManager.h"

#include <cmath>

Player::Player()
{
    isFacingLeft = false;
    // 初始位置和逻辑高度
    position = { 250, 500 };
    logicHeight = 120;

    // 攻击碰撞箱和受击碰撞箱的初始化
    hitBox->setSize({ 150, 150 });
    hurtBox->setSize({ 40, 80 });

    hitBox->setLayerSrc(CollisionLayer::None);
    hitBox->setLayerDst(CollisionLayer::Enemy);

    hurtBox->setLayerSrc(CollisionLayer::Player);
    hurtBox->setLayerDst(CollisionLayer::None);

    // 攻击碰撞箱只有在攻击的时候才会使用
    hitBox->setEnabled(false);
    // 设置受击碰撞箱的回调逻辑是减少生命值 观察者模式
    hurtBox->setOnCollide([&]()
        {
            decreaseHp();
        });
    
    // 翻滚闪避冷却、攻击冷却定时器的初始化
    timerRollCd.setWaitTime(CD_ROLL);
    timerRollCd.setOneShot(true);
    timerRollCd.setOnTimeout([&]()
        {
            isRollCdComp = true;
        });

    timerAttackCd.setWaitTime(CD_ATTACK);
    timerAttackCd.setOneShot(true);
    timerAttackCd.setOnTimeout([&]()
        {
            isAttackCdComp = true;
        });
    
    // 动画对象的初始化

    {
        // attack
        {
            AnimationGroup& animationGroup = animationPool["attack"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_attack_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_attack_right"), 5);
        }

        // dead
        {
            AnimationGroup& animationGroup = animationPool["dead"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_dead_left"), 6);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_dead_right"), 6);
        }
        
        // fall
        {
            AnimationGroup& animationGroup = animationPool["fall"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.15f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_fall_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.15f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_fall_right"), 5);
        }

        // idle
        {
            AnimationGroup& animationGroup = animationPool["idle"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.15f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_idle_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.15f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_idle_right"), 5);
        }

        // jump
        {
            AnimationGroup& animationGroup = animationPool["jump"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.15f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_jump_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.15f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_jump_right"), 5);
        }

        // roll
        {
            AnimationGroup& animationGroup = animationPool["roll"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_roll_left"), 7);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_roll_right"), 7);  
        }

        // run
        {
            AnimationGroup& animationGroup = animationPool["run"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.075f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(ResourcesManager::instance()->findImage("player_run_left"), 10);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.075f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(ResourcesManager::instance()->findImage("player_run_right"), 10);  
        }
    }

    // 特有的特效动画对象初始化
    animationSlashUp.setInterval(0.07f);
    animationSlashUp.setLoop(false);
    animationSlashUp.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashUp.addFrame(ResourcesManager::instance()->findImage("player_vfx_attack_up"), 5);

    animationSlashDown.setInterval(0.07f);
    animationSlashDown.setLoop(false);
    animationSlashDown.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashDown.addFrame(ResourcesManager::instance()->findImage("player_vfx_attack_down"), 5);

    animationSlashLeft.setInterval(0.07f);
    animationSlashLeft.setLoop(false);
    animationSlashLeft.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashLeft.addFrame(ResourcesManager::instance()->findImage("player_vfx_attack_left"), 5);


    animationSlashRight.setInterval(0.07f);
    animationSlashRight.setLoop(false);
    animationSlashRight.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashRight.addFrame(ResourcesManager::instance()->findImage("player_vfx_attack_right"), 5);

    animationJumpVfx.setInterval(0.05f);
    animationJumpVfx.setLoop(false);
    animationJumpVfx.setAnchorMode(Animation::AnchorMode::BottomCentered);
    animationJumpVfx.addFrame(ResourcesManager::instance()->findImage("player_vfx_jump"), 5);
    animationJumpVfx.setOnFinished([&](){ this->isJumpVfxVisible = false; });

    animationLandVfx.setInterval(0.1f);
    animationLandVfx.setLoop(false);
    animationLandVfx.setAnchorMode(Animation::AnchorMode::BottomCentered);
    animationLandVfx.addFrame(ResourcesManager::instance()->findImage("player_vfx_land"), 2);
    animationLandVfx.setOnFinished([&](){ this->isLandVfxVisible = false; });

    {
        stateMachine.registerState("attack", new PlayerAttackState());
        stateMachine.registerState("dead",   new PlayerDeadState());
        stateMachine.registerState("fall",   new PlayerFallState());
        stateMachine.registerState("idle",   new PlayerIdleState());
        stateMachine.registerState("jump",   new PlayerJumpState());
        stateMachine.registerState("roll",   new PlayerRollState());
        stateMachine.registerState("run",    new PlayerRunState());

        stateMachine.setEntry("idle");
    }
}

Player::~Player() = default;

void Player::onInput(const ExMessage& msg)
{
    if (hp <= 0) return;

    switch (msg.message)
    {
    case WM_KEYDOWN:
        switch (msg.vkcode)
        {
        case 0x41:  // A
        case VK_LEFT:  
            isLeftKeyDown = true;
            break;
        case 0x44:  // D
        case VK_RIGHT:
            isRightKeyDown = true;
            break;
        case 0x57:  // w
        case VK_UP: 
            isUpKeyDown = true;
            break;
        case VK_SPACE: 
            isJumpKeyDown = true;
            break;
        case 0x53:  // s
            isDownKeyDown = true;
            break;
        case VK_SHIFT:
            isRollKeyDown = true;
            break;
        case 0x4a:  // j
            isAttackKeyDown = true;
            updateAttackDir();
            break;
        case 0x49:  // i
            playAudio(_T("bullet_time"), false);
            BulletTimeManager::instance()->setStatus(BulletTimeManager::Status::Entering);
        default:
            break;
        }
        break;
    case WM_KEYUP:
        switch (msg.vkcode)
        {
        case 0x41:  // A
        case VK_LEFT:  
            isLeftKeyDown = false;
            break;
        case 0x44:  // D
        case VK_RIGHT:
            isRightKeyDown = false;
            break;
        case 0x57:  // w
        case VK_UP: 
            isUpKeyDown = false;
            break;
        case VK_SPACE: 
            isJumpKeyDown = false;
            break;
        case 0x53:  // s
            isDownKeyDown = false;
            break;
        case VK_SHIFT:
            isRollKeyDown = false;
            break;
        case 0x4a:  // j
            isAttackKeyDown = false;
            break;
        case 0x49:  // i
            playAudio(_T("bullet_time"));
            BulletTimeManager::instance()->setStatus(BulletTimeManager::Status::Exiting);
        default:
            break;
        }
        break;
    
    // 鼠标攻击保留 -> j
    case WM_LBUTTONDOWN:
        isAttackKeyDown = true;
        updateAttackDir(msg.x, msg.y);
        break;
    case WM_LBUTTONUP:
        isAttackKeyDown = false;
        break;
    
    // 鼠标子弹时间保留 -> i
    case WM_RBUTTONDOWN:
        playAudio(_T("bullet_time"), false);
        BulletTimeManager::instance()->setStatus(BulletTimeManager::Status::Entering);
        break;
    case WM_RBUTTONUP:
        playAudio(_T("bullet_time"));
        BulletTimeManager::instance()->setStatus(BulletTimeManager::Status::Exiting);
        break;
    default:
        break;
    }
}

void Player::onUpdate(float delta)
{
    if (hp > 0 && !isRolling)
        velocity.x = getMoveAxis() * SPEED_RUN;

    if (getMoveAxis() != 0)
        isFacingLeft = (getMoveAxis() < 0);
    
    timerRollCd.onUpdate(delta);
    timerAttackCd.onUpdate(delta);

    animationJumpVfx.onUpdate(delta);
    animationLandVfx.onUpdate(delta);

    if (isAttacking)
    {
        currentSlashAnimation->setPosition(getLogicCenter());
        currentSlashAnimation->onUpdate(delta);
    }

    Character::onUpdate(delta);
}

void Player::onRender()
{
    if (isJumpVfxVisible)
        animationJumpVfx.onRender();
    if (isLandVfxVisible)
        animationLandVfx.onRender();
    
    Character::onRender();

    if (isAttacking)
        currentSlashAnimation->onRender();
}

void Player::onHurt()
{
    playAudio(_T("player_hurt"), false);
}

void Player::onJump()
{
    velocity.y -= SPEED_JUMP;
    isJumpVfxVisible = true;
    animationJumpVfx.setPosition(position);
    animationJumpVfx.reset();
}

void Player::onLand()
{
    isLandVfxVisible = true;
    animationLandVfx.setPosition(position);
    animationLandVfx.reset();
}

void Player::onRoll()
{
    timerRollCd.restart();
    isRollCdComp = false;
    velocity.x = isFacingLeft ? -SPEED_ROLL : SPEED_ROLL;
}

void Player::onAttack()
{
    timerAttackCd.restart();
    isAttackCdComp = false;
    switch (attackDir)
    {
    case Player::AttackDir::Up:
        currentSlashAnimation = &animationSlashUp;
        break;
    case Player::AttackDir::Down:
        currentSlashAnimation = &animationSlashDown;
        break;
    case Player::AttackDir::Left:
        currentSlashAnimation = &animationSlashLeft;
        break;
    case Player::AttackDir::Right:
        currentSlashAnimation = &animationSlashRight;
        break;
    default:
        break;
    }
    currentSlashAnimation->setPosition(getLogicCenter());
    currentSlashAnimation->reset();
}

// 根据鼠标位置更新Player攻击方向
void Player::updateAttackDir(int x, int y)
{
    static const float PI = 3.141592654f;
    float angle = std::atan2(y - position.y, x - position.x);  // [-PI, PI]

    if (angle > -PI / 4 && angle < PI / 4)
        attackDir = AttackDir::Right;
    else if (angle >= PI / 4 && angle < 3 * PI / 4)
        attackDir = AttackDir::Down;
    else if ((angle >= 3 * PI && angle <= PI) || (angle >= -PI && angle < -3 * PI / 4))
        attackDir = AttackDir::Left;
    else
        attackDir = AttackDir::Up;
}

// 根据按键控制Player攻击方向
void Player::updateAttackDir()
{
    if (isLeftKeyDown)
        attackDir = AttackDir::Left;
    else if (isRightKeyDown)
        attackDir = AttackDir::Right;
    else if (isUpKeyDown)
        attackDir = AttackDir::Up;
    else if (isDownKeyDown)
        attackDir = AttackDir::Down;
    else
    {
        // 攻击时没有按住方向键
        if (isFacingLeft)
            attackDir = AttackDir::Left;
        else
            attackDir = AttackDir::Right;
    }
}