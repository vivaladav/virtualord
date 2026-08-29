#pragma once

#include "BaseGameState.h"

namespace game
{

enum StateId : unsigned int;

// -- STATE DATA --
class StateDataLoadGame : public sgl::utilities::StateData
{
public:
    StateDataLoadGame(StateId nextState): mNextState(nextState) { }

    StateId GetNextState() const { return mNextState; }

private:
    StateId mNextState;
};

// -- STATE --

class StateLoadGame : public BaseGameState
{
public:
    StateLoadGame(Game * game);

    void SetNextStateData(sgl::utilities::StateData * data) override;

private:
    void OnActive() override;
    void OnInactive() override;

private:
    StateId mNextState;
};

} // namespace game
