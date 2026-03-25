#pragma once

#include "States/BaseGameState.h"

namespace game
{

enum StateId : int;

// -- STATE DATA --
class StateDataLeavePregame : public sgl::utilities::StateData
{
public:
    StateDataLeavePregame(StateId nextState);

    StateId GetNextState() const;

private:
    StateId mNextState;
};

inline StateDataLeavePregame::StateDataLeavePregame(StateId nextState)
    : mNextState(nextState)
{
}

inline StateId StateDataLeavePregame::GetNextState() const { return mNextState; }

// -- STATE --
class StateLeavePregame : public BaseGameState
{
public:
    StateLeavePregame(Game * game);

    void SetNextStateData(sgl::utilities::StateData * data) override;

private:
    void OnActive() override;
    void OnInactive() override;

    void DestroyPregameTextures();

private:
    StateId mNextState;
};

} // namespace game
