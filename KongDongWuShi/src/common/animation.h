#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "util.h"
#include "timer.h"
#include "atlas.h"
#include "vector2.h"

#include <vector>
#include <functional>

class Animation
{
public:
    // 居中锚点、底部居中锚点
    enum class AnchorMode
    {
        Centered,
        BottomCentered
    };
public:
    Animation()
    {
        timer.setOneShot(false);
        timer.setOnTimeout(
            [&]()
            {
                idxFrame++;
                if (idxFrame >= frameList.size())
                {
                    idxFrame = isLoop ? 0 : frameList.size() - 1;
                    // BUG: 这里如果是不循环动画，这里动画不停止，此回调岂不是达到最后后一直调用？
                    if (!isLoop && onFinished)
                        onFinished();
                }
            }
        );
    }

    ~Animation() = default;

    /// @brief 重置动画
    void reset()
    {
        timer.restart();

        idxFrame = 0;
    }

    /// @brief 设置动画锚点
    /// @param mode 锚点
    void setAnchorMode(AnchorMode mode)
    {
        anchorMode = mode;
    }

    /// @brief 设置动画位置
    /// @param position 位置坐标
    void setPosition(const Vector2& position)
    {
        this->position = position;
    }

    /// @brief 设置是否循环
    /// @param isLoop 循环?
    void setLoop(bool isLoop)
    {
        this->isLoop = isLoop;
    }

    /// @brief 设置动画在多少s后播放一帧
    /// @param interval s
    void setInterval(float interval)
    {
        timer.setWaitTime(interval);
    }

    /// @brief 设置动画结束后执行的回调(Loop无效)
    /// @param onFinished 执行回调逻辑
    void setOnFinished(std::function<void()> onFinished)
    {
        this->onFinished = onFinished;
    }

    /// @brief 添加动画帧
    /// @param image 图片对象
    /// @param numH 均分个数(对于一个图片存在多帧情况)
    /// @warning numH需要>=1，不可等于0
    void addFrame(IMAGE* image, int numH)
    {
        int width = image->getwidth();
        int height = image->getheight();
        int widthFrame = width / numH;

        for (int i = 0; i < numH; ++i)
        {
            Rect rectSrc;
            rectSrc.x = i * widthFrame, rectSrc.y = 0;
            rectSrc.w = widthFrame, rectSrc.h = height;

            frameList.emplace_back(image, rectSrc);
        }
    }

    /// @brief 添加动画帧
    /// @param atlas 图集(其中一个图片对象就是一个动画帧)
    void addFrame(Atlas* atlas)
    {
        for (int i = 0; i < atlas->getSize(); ++i)
        {
            IMAGE* image = atlas->getImage(i);
            int width = image->getwidth();
            int height = image->getheight();

            Rect rectSrc;
            rectSrc.x = 0, rectSrc.y = 0;
            rectSrc.w = width, rectSrc.h = height;

            frameList.emplace_back(image, rectSrc);
        }
    }

    /// @brief 每帧更新
    /// @param delta 帧率
    void onUpdate(float delta)
    {
        timer.onUpdate(delta);
    }

    /// @brief 渲染
    void onRender()
    {
        const Frame& frame = frameList[idxFrame];

        Rect rectDst;
        rectDst.x = static_cast<int>(position.x) - frame.rectSrc.w / 2;
        rectDst.y = (anchorMode == AnchorMode::Centered)
            ? static_cast<int>(position.y) - frame.rectSrc.h / 2 : static_cast<int>(position.y) - frame.rectSrc.h;
        rectDst.w = frame.rectSrc.w, rectDst.h = frame.rectSrc.h;

        putimageEx(frame.img, &rectDst, &frame.rectSrc);
    }
private:
    // 描述动画播放过程中的"一帧"
    struct Frame
    {
        Rect rectSrc;
        IMAGE* img = nullptr;

        Frame() = default;
        Frame(IMAGE* image, const Rect& rectSrc)
            :img(image), rectSrc(rectSrc) {}
        ~Frame() = default;
    };
private:
    Timer timer;                                        // 定时器
    Vector2 position;                                   // 动画在窗口渲染的位置
    bool isLoop = true;                                 // 设置动画是否需要循环播放
    size_t idxFrame = 0;                                // 当前动画播放的帧索引
    std::vector<Frame> frameList;                       // 动画包含每一帧的帧信息
    std::function<void()> onFinished;                   // 动画播放结束后的逻辑(驱动力)
    AnchorMode anchorMode = AnchorMode::Centered;       // 当前动画的锚点模式
};


#endif