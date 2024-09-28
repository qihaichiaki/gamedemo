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
        timer.setOneTimeout(
            [&]()
            {
                idxFrame++;
                if (idxFrame >= frameList.size())
                {
                    idxFrame = isLoop ? 0 : frameList.size() - 1;
                    if (!isLoop && onFinished)
                        onFinished();
                }
            }
        );
    }

    ~Animation() = default;

    // 重置动画
    void reset()
    {
        timer.restart();

        idxFrame = 0;
    }

    void setAnchorMode(AnchorMode mode)
    {
        anchorMode = mode;
    }

    void setPosition(const Vector2& position)
    {
        this->position = position;
    }

    void setLoop(bool isLoop)
    {
        this->isLoop = isLoop;
    }

    void setInterval(float interval)
    {
        timer.setWaitTime(interval);
    }

    void setOnFinished(std::function<void()> onFinished)
    {
        this->onFinished = onFinished;
    }

    // 添加动画帧
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

    // 更新+渲染
    void onUpdate(float delta)
    {
        timer.onUpdate(delta);
    }

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