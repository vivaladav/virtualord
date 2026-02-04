#pragma once

#include "States/BaseGameState.h"

namespace game
{

class StateLeaveGame : public BaseGameState
{
public:
    StateLeaveGame(Game * game);

private:
    void OnActive() override;
    void OnInactive() override;

    void DestroyTextures();
};

} // namespace game
