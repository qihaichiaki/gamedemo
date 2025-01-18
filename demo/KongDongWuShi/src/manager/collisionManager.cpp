#include "collisionManager.h"

#include <graphics.h>

CollisionManager* CollisionManager::instance()
{
    static CollisionManager manager;
    return &manager;
}

CollisionManager::CollisionManager() = default;

CollisionManager::~CollisionManager() = default;

CollisionBox* CollisionManager::createCollisionBox()
{
    CollisionBox* collisionBox = new CollisionBox();

    collisionBoxList.push_back(collisionBox);

    return collisionBox;
}

void CollisionManager::destroyCollisionBox(CollisionBox* collisionBox)
{
    collisionBoxList.erase(std::remove(collisionBoxList.begin(),
        collisionBoxList.end(), collisionBox), collisionBoxList.end());
    
    delete collisionBox;
}

// 碰撞检测
// 如果碰撞到了相互执行onCollide函数回调
void CollisionManager::processCollide()
{
    for (CollisionBox* collisionBoxSrc : collisionBoxList)
    {
        if (!collisionBoxSrc->enabled || collisionBoxSrc->layerDst == CollisionLayer::None)
            continue;
        for (CollisionBox* collisionBoxDst : collisionBoxList)
        {
            if (!collisionBoxDst->enabled || collisionBoxSrc == collisionBoxDst
                || collisionBoxSrc->layerDst != collisionBoxDst->layerSrc)
                continue;
            
            bool isCollideX = (max(collisionBoxSrc->position.x + collisionBoxDst->size.x / 2,
                collisionBoxDst->position.x + collisionBoxDst->size.x / 2)
                - min(collisionBoxSrc->position.x - collisionBoxSrc->size.x / 2, 
                      collisionBoxDst->position.x - collisionBoxDst->size.x / 2)
                <= collisionBoxSrc->size.x + collisionBoxDst->size.x);
            bool isCollideY = (max(collisionBoxSrc->position.y + collisionBoxDst->size.y / 2,
                collisionBoxDst->position.y + collisionBoxDst->size.y / 2)
                - min(collisionBoxSrc->position.y - collisionBoxSrc->size.y / 2, 
                      collisionBoxDst->position.y - collisionBoxDst->size.y / 2)
                <= collisionBoxSrc->size.y + collisionBoxDst->size.y);
            
            if (isCollideX && isCollideY && collisionBoxDst->onCollide)
                collisionBoxDst->onCollide();
        }
    }
}

void CollisionManager::onDebugRender()
{
    for (CollisionBox* collisionBox : collisionBoxList)
    {
        setlinecolor(collisionBox->enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
        rectangle(static_cast<int>(collisionBox->position.x - collisionBox->size.x / 2),
            static_cast<int>(collisionBox->position.y - collisionBox->size.y / 2),
            static_cast<int>(collisionBox->position.x + collisionBox->size.x / 2),
            static_cast<int>(collisionBox->position.y + collisionBox->size.y / 2));
    }
}