#ifndef _COLLISION_BOX_H_
#define _COLLISION_BOX_H_

#include "vector2.h"
#include "collisionLayer.h"

#include <functional>

class CollisionManager;

// 矩形碰撞箱
class CollisionBox
{
    friend class CollisionManager;
    using Collide = std::function<void()>;
private:
    Vector2 size;  // 尺寸
    Vector2 position;  // 中心点
    bool enabled = true;  // 是否启用碰撞检测
    Collide onCollide;  // 碰撞发生后的回调逻辑
    CollisionLayer layerSrc = CollisionLayer::None;  // 当前所处碰撞层
    CollisionLayer layerDst = CollisionLayer::None;  // 碰撞对方的碰撞层

private:
    CollisionBox() = default;
    ~CollisionBox() = default;

public:
    void setEnabled(bool flag)
    {
        enabled = flag;
    }

    void setLayerSrc(CollisionLayer layer)
    {
        layerSrc = layer;
    }

    void setLayerDst(CollisionLayer layer)
    {
        layerDst = layer;
    }

    void setOnCollide(Collide onCollide)
    {
        this->onCollide = onCollide;
    }

    void setSize(const Vector2& size)
    {
        this->size = size;
    }

    const Vector2& getSize() const
    {
        return size;
    }

    void setPosition(const Vector2& position)
    {
        this->position = position;
    }
};

#endif