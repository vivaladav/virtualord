#pragma once

#include "States/BaseGameState.h"

namespace game
{

class StateLeavePregame : public BaseGameState
{
public:
    StateLeavePregame(Game * game);

private:
    void OnActive() override;
    void OnInactive() override;
};

} // namespace game
