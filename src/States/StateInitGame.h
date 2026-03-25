#pragma once

#include "BaseGameState.h"

namespace game
{

class StateInitGame : public BaseGameState
{
public:
    StateInitGame(Game * game);

private:
    void OnActive() override;
    void OnInactive() override;
};

} // namespace game
