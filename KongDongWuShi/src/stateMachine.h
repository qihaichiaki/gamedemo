#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include "stateNode.h"

#include <string>
#include <graphics.h>
#include <unordered_map>

class StateMachine
{
public:
    StateMachine();
    ~StateMachine();

    void onUpdate(float delta);

    // 初始状态
    void setEntry(const std::string& id);
    // 切换状态机的激活状态
    void switchTo(const std::string& id);
    // 向状态机中注册新的状态
    void registerState(const std::string& id, StateNode* stateNode);

private:
    bool needInit = true;
    StateNode* currentState = nullptr;
    std::unordered_map<std::string, StateNode*> statePool;
};


#endif