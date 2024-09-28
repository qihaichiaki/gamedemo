#include "barb.h"
#include "collisionManager.h"
#include "resourcesManager.h"
#include "characterManager.h"

Barb::Barb()
{
    diffPeriod = rangeRandom(0, 6);

    animationLoose.setInterval(0.15f);
    animationLoose.setLoop(true);
    animationLoose.setAnchorMode(Animation::AnchorMode::Centered);
    animationLoose.addFrame(ResourcesManager::instance()->findAtlas("barb_loose"));

    animationBreak.setInterval(0.1f);
    animationBreak.setLoop(false);
    animationBreak.setAnchorMode(Animation::AnchorMode::Centered);
    animationBreak.addFrame(ResourcesManager::instance()->findAtlas("barb_break"));
    animationBreak.setOnFinished([&](){ isValid = false; });

    collisionBox = CollisionManager::instance()->createCollisionBox();
    collisionBox->setLayerSrc(CollisionLayer::Enemy);
    collisionBox->setLayerDst(CollisionLayer::Player);
    collisionBox->setSize({ 20, 20 });
    collisionBox->setOnCollide([&](){ onBreak(); });

    timerIdle.setWaitTime((float)rangeRandom(3, 10));
    timerIdle.setOneShot(true);
    timerIdle.setOneTimeout([&]()
        {
            if (stage == Stage::Idle)
            {
                stage = Stage::Aim;
                basePosition = currentPosition;
            }
        });
    
    timerAim.setWaitTime(0.75f);
    timerAim.setOneShot(true);
    timerAim.setOneTimeout([&]()
        {  
            if (stage == Stage::Aim)
            {
                stage = Stage::Dash;
                const Vector2& posPlayer = CharacterManager::instance()->getPlayer()->getPosition();
                velocity = (posPlayer - currentPosition).normalize() * SPEED_DASH;
            }  
        });
}

Barb::~Barb()
{
    CollisionManager::instance()->destroyCollisionBox(collisionBox);
}

void Barb::onUpdate(float delta)
{
    // 更新定时器逻辑
    if (stage == Stage::Idle)
        timerIdle.onUpdate(delta);
    if (stage == Stage::Aim)
        timerAim.onUpdate(delta);
    
    // 更新移动逻辑
    totalDeltaTime += delta;
    switch (stage)
    {
    case Barb::Stage::Idle:
        currentPosition.y = basePosition.y + sin(totalDeltaTime * 2 + diffPeriod) * 30;
        break;
    case Barb::Stage::Aim:
        currentPosition.x = basePosition.x + rangeRandom(-10, 10);
        break;
    case Barb::Stage::Dash:
        currentPosition += velocity * delta;
        if (currentPosition.y >= CharacterManager::instance()->getPlayer()->getFloorY())
            onBreak();
        if (currentPosition.y <= 0)
            isValid = false;
        break;
    }
    collisionBox->setPosition(currentPosition);

    // 更新动画逻辑
    currentAnimation = (stage == Stage::Break ? &animationBreak : &animationLoose);
    currentAnimation->setPosition(currentPosition);
    currentAnimation->onUpdate(delta);
}

void Barb::onRender()
{
    currentAnimation->onRender();
}

void Barb::onBreak()
{
    if (stage == Stage::Break) return;

    stage = Stage::Break;
    collisionBox->setEnabled(false);
    playAudio(_T("barb_break"), false);
}