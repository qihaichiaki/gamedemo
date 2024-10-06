#include "stateMachine.h"

StateMachine::StateMachine() = default;
StateMachine::~StateMachine() = default;

void StateMachine::onUpdate(float delta)
{
    if (!currentState) return;

    if (needInit)
    {
        currentState->onEnter();
        needInit = false;
    }

    currentState->onUpdate(delta);
}

void StateMachine::setEntry(const std::string& id)
{
    currentState = statePool[id];
}

void StateMachine::switchTo(const std::string& id)
{
    if (currentState) currentState->onExit();
    currentState = statePool[id];
    if (currentState) currentState->onEnter();
}

void StateMachine::registerState(const std::string& id, StateNode* stateNode)
{
    statePool[id] = stateNode;
}