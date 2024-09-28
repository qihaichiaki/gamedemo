#include "bulletTimeManager.h"

#include <graphics.h>

BulletTimeManager* BulletTimeManager::instance()
{
    static BulletTimeManager manager;
    return &manager;
}

void BulletTimeManager::postProcess()
{
    DWORD* buffer = GetImageBuffer();
    int w = getwidth(), h = getheight();
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int idx = y * w + x;
            DWORD color = buffer[idx];
            BYTE r = (BYTE)(GetBValue(color) * lerp(1.0f, DST_COLOR_FACTOR, progress));
            BYTE g = (BYTE)(GetGValue(color) * lerp(1.0f, DST_COLOR_FACTOR, progress));
            BYTE b = (BYTE)(GetRValue(color) * lerp(1.0f, DST_COLOR_FACTOR, progress));
            buffer[idx] = BGR(RGB(r, g, b)) | ((DWORD)((BYTE)(255)) << 24);
        }
    }
}

void BulletTimeManager::setStatus(Status status) { this->status = status; }

/// @brief 计算帧缩放后的帧更新时间
/// @param delta 
/// @return 
float BulletTimeManager::onUpdate(float delta)
{
    float deltaProgress = SPEED_PROGRESS * delta;
    progress += deltaProgress * (status == Status::Entering ? 1 : -1);

    if (progress < 0) progress = 0;
    if (progress > 1.0f) progress = 1.0f;

    return delta * lerp(1.0f, DST_DELTA_FACTOR, progress); 
}

BulletTimeManager::BulletTimeManager() = default;
BulletTimeManager::~BulletTimeManager() = default;
BulletTimeManager::BulletTimeManager(const BulletTimeManager&) = default;