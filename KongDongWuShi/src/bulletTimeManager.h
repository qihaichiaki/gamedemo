#ifndef _BULLET_TIME_MANAGER_H_
#define _BULLET_TIME_MANAGER_H_

// 子弹时间管理器

class BulletTimeManager
{
public:
    // 标注现在子弹时间状态
    enum class Status
    {
        Entering,
        Exiting
    };

public:
    static BulletTimeManager* instance();

    // 全屏处理，除开玩家其余暗了下去
    void postProcess();
    void setStatus(Status status);

    float onUpdate(float delta);

private:
    // 进入子弹时间效果的进度 0没有进入子弹时间 1完全进入了
    float progress = 0;
    Status status = Status::Exiting;
    const float SPEED_PROGRESS = 2.0f;  // 进入、退出速度
    const float DST_DELTA_FACTOR = 0.35f;  // 子弹时间的帧更新时间的缩放
    const float DST_COLOR_FACTOR = 0.35f;  // 子弹时间的画面色彩改变比例

private:
    BulletTimeManager();
    ~BulletTimeManager();
    BulletTimeManager(const BulletTimeManager&);

    /// @brief 插值 计算区间[start, end] 区间内进度为t(0~100%)的实际数值
    /// @param start 
    /// @param end 
    /// @param t 
    /// @return 
    float lerp(float start, float end, float t)
    {
        return (1 - t) * start + t * end;  // (end - start) * t + start
    }
};

#endif