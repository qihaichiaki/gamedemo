#ifndef _COLLISION_MANAGER_H_
#define _COLLISION_MANAGER_H_

#include "collisionBox.h"

#include <vector>

class CollisionManager
{
public:
    static CollisionManager* instance();

    CollisionBox* createCollisionBox();
    void destroyCollisionBox(CollisionBox* collisionBox);

    void processCollide();
    void onDebugRender();
private:
    std::vector<CollisionBox*> collisionBoxList;
private:
    CollisionManager();
    ~CollisionManager();
};

#endif // !_COLLISION_MANAGER_H_