#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vector2.hpp"

class Camera
{
public:
    Camera() = default;
    ~Camera() = default;

    void setSize(const Vector2& size) { this->size = size; }

    const Vector2& getSize() const { return size; }

    void setPosition(const Vector2& position) { this->position = position; }

    const Vector2& getPosition() const { return position; }

    void lookAt(const Vector2& target) { position = target - size / 2.0f; }

private:
    Vector2 size;      // 相机大小
    Vector2 position;  // 相机位置
};

#endif