#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "barb.h"
#include "sword.h"
#include "character.h"

class Enemy: public Character
{
public:
    Enemy();
    ~Enemy();

    void onUpdate(float delta) override;
    void onRender() override;

    void onHurt() override;

    void setFacingLeft(bool flag) { isFacingLeft = flag; };
    bool getFacingLeft() const { return isFacingLeft; }

    void setDashingInAir(bool flag) { isDashingInAir = flag; }
    bool getDashingInAir() const { return isDashingInAir; }

    void setDashingOnFloor(bool flag) { isDashingOnFloor = flag; }
    bool getDashingOnFloor() const { return isDashingOnFloor; }
    
    void setThrowingSilk(bool flag)
    {
        isThrowingSilk = flag;
        collisionBoxSilk->setEnabled(flag);
    }
    bool getThrowingSilk() const { return isThrowingSilk; }

    void throwBarbs();
    void throwSword();

    void onDash();
    void onThrowSilk();

private:
    bool isDashingInAir = false;
    bool isDashingOnFloor = false;
    bool isThrowingSilk = false;

    Animation animationSilk;
    AnimationGroup animationDashInAirVfx;
    AnimationGroup animationDashOnFloorVfx;
    Animation* currentDashAnimation = nullptr;
    
    std::vector<Barb*> barbList;
    std::vector<Sword*> swordList;
    CollisionBox* collisionBoxSilk = nullptr;
};

#endif