#ifndef _STATE_NODE_H_
#define _STATE_NODE_H_

class StateNode
{
public:
    StateNode() = default;
    ~StateNode() = default;

    virtual void onEnter() { }
    virtual void onUpdate(float delta) { }
    virtual void onExit() { }
};


#endif