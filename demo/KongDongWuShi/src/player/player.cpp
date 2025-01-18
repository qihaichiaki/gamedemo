#include "player.h"

#include <cmath>

#include "bulletTimeManager.h"
#include "playerStateNodes.h"
#include "resourcesManager.h"

Player::Player() {
    isFacingLeft = false;
    // 初始位置和逻辑高度
    position = {250, 500};
    logicHeight = 120;

    // 攻击碰撞箱和受击碰撞箱的初始化
    hitBox->setSize({150, 150});
    hurtBox->setSize({40, 80});

    hitBox->setLayerSrc(CollisionLayer::None);
    hitBox->setLayerDst(CollisionLayer::Enemy);

    hurtBox->setLayerSrc(CollisionLayer::Player);
    hurtBox->setLayerDst(CollisionLayer::None);

    // 攻击碰撞箱只有在攻击的时候才会使用
    hitBox->setEnabled(false);
    // 设置受击碰撞箱的回调逻辑是减少生命值 观察者模式
    hurtBox->setOnCollide([&]() { decreaseHp(); });

    // 翻滚闪避冷却、攻击冷却定时器的初始化
    timerRollCd.setWaitTime(CD_ROLL);
    timerRollCd.setOneShot(true);
    timerRollCd.setOnTimeout([&]() { isRollCdComp = true; });

    timerAttackCd.setWaitTime(CD_ATTACK);
    timerAttackCd.setOneShot(true);
    timerAttackCd.setOnTimeout([&]() { isAttackCdComp = true; });

    // 动画对象的初始化

    {
        // attack
        {
            AnimationGroup& animationGroup = animationPool["attack"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_attack_left"),
                5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_attack_right"),
                5);
        }

        // dead
        {
            AnimationGroup& animationGroup = animationPool["dead"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.1f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_dead_left"), 6);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.1f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_dead_right"),
                6);
        }

        // fall
        {
            AnimationGroup& animationGroup = animationPool["fall"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.15f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_fall_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.15f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_fall_right"),
                5);
        }

        // idle
        {
            AnimationGroup& animationGroup = animationPool["idle"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.15f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_idle_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.15f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_idle_right"),
                5);
        }

        // jump
        {
            AnimationGroup& animationGroup = animationPool["jump"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.15f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_jump_left"), 5);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.15f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_jump_right"),
                5);
        }

        // roll
        {
            AnimationGroup& animationGroup = animationPool["roll"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.05f);
            animationLeft.setLoop(false);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_roll_left"), 7);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.05f);
            animationRight.setLoop(false);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_roll_right"),
                7);
        }

        // run
        {
            AnimationGroup& animationGroup = animationPool["run"];

            Animation& animationLeft = animationGroup.left;
            animationLeft.setInterval(0.075f);
            animationLeft.setLoop(true);
            animationLeft.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationLeft.addFrame(
                ResourcesManager::instance()->findImage("player_run_left"), 10);

            Animation& animationRight = animationGroup.right;
            animationRight.setInterval(0.075f);
            animationRight.setLoop(true);
            animationRight.setAnchorMode(Animation::AnchorMode::BottomCentered);
            animationRight.addFrame(
                ResourcesManager::instance()->findImage("player_run_right"),
                10);
        }
    }

    // 特有的特效动画对象初始化
    animationSlashUp.setInterval(0.07f);
    animationSlashUp.setLoop(false);
    animationSlashUp.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashUp.addFrame(
        ResourcesManager::instance()->findImage("player_vfx_attack_up"), 5);

    animationSlashDown.setInterval(0.07f);
    animationSlashDown.setLoop(false);
    animationSlashDown.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashDown.addFrame(
        ResourcesManager::instance()->findImage("player_vfx_attack_down"), 5);

    animationSlashLeft.setInterval(0.07f);
    animationSlashLeft.setLoop(false);
    animationSlashLeft.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashLeft.addFrame(
        ResourcesManager::instance()->findImage("player_vfx_attack_left"), 5);

    animationSlashRight.setInterval(0.07f);
    animationSlashRight.setLoop(false);
    animationSlashRight.setAnchorMode(Animation::AnchorMode::Centered);
    animationSlashRight.addFrame(
        ResourcesManager::instance()->findImage("player_vfx_attack_right"), 5);

    animationJumpVfx.setInterval(0.05f);
    animationJumpVfx.setLoop(false);
    animationJumpVfx.setAnchorMode(Animation::AnchorMode::BottomCentered);
    animationJumpVfx.addFrame(
        ResourcesManager::instance()->findImage("player_vfx_jump"), 5);
    animationJumpVfx.setOnFinished([&]() { this->isJumpVfxVisible = false; });

    animationLandVfx.setInterval(0.1f);
    animationLandVfx.setLoop(false);
    animationLandVfx.setAnchorMode(Animation::AnchorMode::BottomCentered);
    animationLandVfx.addFrame(
        ResourcesManager::instance()->findImage("player_vfx_land"), 2);
    animationLandVfx.setOnFinished([&]() { this->isLandVfxVisible = false; });

    {
        stateMachine.registerState("attack", new PlayerAttackState());
        stateMachine.registerState("dead", new PlayerDeadState());
        stateMachine.registerState("fall", new PlayerFallState());
        stateMachine.registerState("idle", new PlayerIdleState());
        stateMachine.registerState("jump", new PlayerJumpState());
        stateMachine.registerState("roll", new PlayerRollState());
        stateMachine.registerState("run", new PlayerRunState());

        stateMachine.setEntry("idle");
    }

    // 初始化设置
    auto& leftStrData = ResourcesManager::instance()->findSetData("left");
    auto& rightStrData = ResourcesManager::instance()->findSetData("right");
    auto& upStrData = ResourcesManager::instance()->findSetData("up");
    auto& downStrData = ResourcesManager::instance()->findSetData("down");
    auto& attackStrData = ResourcesManager::instance()->findSetData("attack");
    auto& rollStrData = ResourcesManager::instance()->findSetData("roll");
    auto& jumpStrData = ResourcesManager::instance()->findSetData("jump");
    auto& bulletTimeStrData =
        ResourcesManager::instance()->findSetData("bulletTime");

    leftKey = leftStrData.empty() ? 0x41 : leftStrData.Int();
    rightKey = rightStrData.empty() ? 0x44 : rightStrData.Int();
    upKey = upStrData.empty() ? 0x57 : upStrData.Int();
    downKey = downStrData.empty() ? 0x53 : downStrData.Int();
    attackKey = attackStrData.empty() ? 0x4a : attackStrData.Int();
    rollKey = rollStrData.empty() ? VK_SHIFT : rollStrData.Int();
    jumpKey = jumpStrData.empty() ? VK_SPACE : jumpStrData.Int();
    bulletTimeKey = bulletTimeStrData.empty() ? 0x49 : bulletTimeStrData.Int();
}

Player::~Player() = default;

void Player::onInput(const ExMessage& msg) {
    if (hp <= 0) return;

    switch (msg.message) {
        case WM_KEYDOWN:
            if (msg.vkcode == leftKey)
                isLeftKeyDown = true;
            else if (msg.vkcode == rightKey)
                isRightKeyDown = true;
            else if (msg.vkcode == upKey)
                isUpKeyDown = true;
            else if (msg.vkcode == jumpKey)
                isJumpKeyDown = true;
            else if (msg.vkcode == downKey)
                isDownKeyDown = true;
            else if (msg.vkcode == rollKey)
                isRollKeyDown = true;
            else if (msg.vkcode == attackKey)
            {
                isAttackKeyDown = true;
                updateAttackDir();
            }
            else if (msg.vkcode == bulletTimeKey)
            {
                playAudio(_T("bullet_time"), false);
                BulletTimeManager::instance()->setStatus(
                    BulletTimeManager::Status::Entering);
            }
            break;
        case WM_KEYUP:
            if (msg.vkcode == leftKey)
                isLeftKeyDown = false;
            else if (msg.vkcode == rightKey)
                isRightKeyDown = false;
            else if (msg.vkcode == upKey)
                isUpKeyDown = false;
            else if (msg.vkcode == jumpKey)
                isJumpKeyDown = false;
            else if (msg.vkcode == downKey)
                isDownKeyDown = false;
            else if (msg.vkcode == rollKey)
                isRollKeyDown = false;
            else if (msg.vkcode == attackKey)
                isAttackKeyDown = false;
            else if (msg.vkcode == bulletTimeKey)
            {
                playAudio(_T("bullet_time"));
                BulletTimeManager::instance()->setStatus(
                        BulletTimeManager::Status::Exiting);
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
            BulletTimeManager::instance()->setStatus(
                BulletTimeManager::Status::Entering);
            break;
        case WM_RBUTTONUP:
            playAudio(_T("bullet_time"));
            BulletTimeManager::instance()->setStatus(
                BulletTimeManager::Status::Exiting);
            break;
        default:
            break;
    }
}

void Player::onUpdate(float delta) {
    if (hp > 0 && !isRolling) velocity.x = getMoveAxis() * SPEED_RUN;

    if (getMoveAxis() != 0) isFacingLeft = (getMoveAxis() < 0);

    timerRollCd.onUpdate(delta);
    timerAttackCd.onUpdate(delta);

    animationJumpVfx.onUpdate(delta);
    animationLandVfx.onUpdate(delta);

    if (isAttacking) {
        currentSlashAnimation->setPosition(getLogicCenter());
        currentSlashAnimation->onUpdate(delta);
    }

    Character::onUpdate(delta);
}

void Player::onRender() {
    if (isJumpVfxVisible) animationJumpVfx.onRender();
    if (isLandVfxVisible) animationLandVfx.onRender();

    Character::onRender();

    if (isAttacking) currentSlashAnimation->onRender();
}

void Player::onHurt() { playAudio(_T("player_hurt"), false); }

void Player::onJump() {
    velocity.y -= SPEED_JUMP;
    isJumpVfxVisible = true;
    animationJumpVfx.setPosition(position);
    animationJumpVfx.reset();
}

void Player::onLand() {
    isLandVfxVisible = true;
    animationLandVfx.setPosition(position);
    animationLandVfx.reset();
}

void Player::onRoll() {
    timerRollCd.restart();
    isRollCdComp = false;
    velocity.x = isFacingLeft ? -SPEED_ROLL : SPEED_ROLL;
}

void Player::onAttack() {
    timerAttackCd.restart();
    isAttackCdComp = false;
    switch (attackDir) {
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
void Player::updateAttackDir(int x, int y) {
    static const float PI = 3.141592654f;
    float angle = std::atan2(y - position.y, x - position.x);  // [-PI, PI]

    if (angle > -PI / 4 && angle < PI / 4)
        attackDir = AttackDir::Right;
    else if (angle >= PI / 4 && angle < 3 * PI / 4)
        attackDir = AttackDir::Down;
    else if ((angle >= 3 * PI && angle <= PI) ||
             (angle >= -PI && angle < -3 * PI / 4))
        attackDir = AttackDir::Left;
    else
        attackDir = AttackDir::Up;
}

// 根据按键控制Player攻击方向
void Player::updateAttackDir() {
    if (isLeftKeyDown)
        attackDir = AttackDir::Left;
    else if (isRightKeyDown)
        attackDir = AttackDir::Right;
    else if (isUpKeyDown)
        attackDir = AttackDir::Up;
    else if (isDownKeyDown)
        attackDir = AttackDir::Down;
    else {
        // 攻击时没有按住方向键
        if (isFacingLeft)
            attackDir = AttackDir::Left;
        else
            attackDir = AttackDir::Right;
    }
}