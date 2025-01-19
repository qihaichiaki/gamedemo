#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <animation.hpp>
#include <cassert>
#include <vector2.hpp>

constexpr int FACING_NUM = 4;

class Player
{
public:
    enum Facing { Up, Down, Left, Right };

public:
    Player(Atlas* atlas_idle[], Atlas* atlas_run[])
    {
        assert(sizeof(atlas_idle) == FACING_NUM * sizeof(Atlas*));
        assert(sizeof(atlas_run) == FACING_NUM * sizeof(Atlas*));

        for (int i = 0; i < FACING_NUM; ++i) {
            anim_idle[i].addFrame(atlas_idle[i]);
            anim_run[i].addFrame(atlas_run[i]);

            anim_idle[i].setInterval(0.1f);
            anim_idle[i].setLoop(true);
            anim_run[i].setInterval(0.1f);
            anim_run[i].setLoop(true);
        }
    }

    ~Player() = default;

    void onUpdate(float delta)
    {
        if (!position.approx(pos_target))
            velocity = (pos_target - position).normalize() * SPEED_RUN;
        else
            velocity = Vector2(0, 0);

        if ((pos_target - position).length() > SPEED_RUN * delta)
            position += velocity * delta;
        else
            position = pos_target;

        if (velocity.approx(Vector2(0, 0))) {
            switch (current_facing) {
                case Up:
                    current_anim = &anim_idle[Up];
                    break;
                case Down:
                    current_anim = &anim_idle[Down];
                    break;
                case Left:
                    current_anim = &anim_idle[Left];
                    break;
                case Right:
                    current_anim = &anim_idle[Right];
                    break;
            }
        } else {
            if (abs(velocity.y) >= 0.0001f)
                current_facing = velocity.y > 0 ? Down : Up;
            else if (abs(velocity.x) >= 0.0001f)
                current_facing = velocity.x > 0 ? Right : Left;

            switch (current_facing) {
                case Up:
                    current_anim = &anim_run[Up];
                    break;
                case Down:
                    current_anim = &anim_run[Down];
                    break;
                case Left:
                    current_anim = &anim_run[Left];
                    break;
                case Right:
                    current_anim = &anim_run[Right];
                    break;
            }
        }

        if (!current_anim) return;
        current_anim->setPosition(position);
        current_anim->onUpdate(delta);
    }

    void on_render(const Camera& camera)
    {
        if (!current_anim) return;
        current_anim->onRender(camera);
    }

    void setPosition(const Vector2& position) { this->position = position; }

    const Vector2& getPosition() const { return position; }

    void setTarget(const Vector2& pos_target) { this->pos_target = pos_target; }

private:
    const float SPEED_RUN = 100.0f;  // 每秒100像素距离?

private:
    Vector2 position;
    Vector2 velocity;
    Vector2 pos_target;

    Animation anim_idle[FACING_NUM];
    Animation anim_run[FACING_NUM];
    Animation* current_anim = nullptr;

    Facing current_facing = Down;
};

#endif