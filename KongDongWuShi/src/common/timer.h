#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>

class Timer
{
public:
    Timer() = default;
    ~Timer() = default;

    /// @brief 重新开始
    void restart()
    {
        passTime = 0;
        shotted = false;
    }

    /// @brief 设置计时器等待时间
    /// @param val 等待时间s
    void setWaitTime(float val) { waitTime = val; }

    /// @brief 单次触发回调
    /// @param flag 是否单次触发
    void setOneShot(bool flag) { oneShot = flag; }

    /// @brief 设置回调
    /// @param onTimeout void() 
    void setOnTimeout(std::function<void()> onTimeout) { this->onTimeout = onTimeout; }

    /// @brief 暂停
    void pause() { paused = true; }

    /// @brief 继续
    void resume() { paused = false; }

    /// @brief 定时器更新
    /// @param delta 时间帧
    void onUpdate(float delta)
    {
        if (paused) return;

        passTime += delta;
        if (passTime >= waitTime)
        {
            bool canShot = (!oneShot || (oneShot && !shotted));
            shotted = true;
            if(canShot && onTimeout)
                onTimeout();
            passTime -= waitTime;
        }
    }
private:
    float passTime = 0;                 // 当前过去的时间
    float waitTime = 0;                 // 需要等待的时间
    bool paused = false;                // 是否暂停
    bool shotted = false;               // 回调是否执行过一次
    bool oneShot = false;               // 是否单次触发回调
    std::function<void()> onTimeout;    // 回调函数
};

#endif // !_TIMER_H_