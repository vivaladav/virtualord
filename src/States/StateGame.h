#pragma once

#include "BaseGameState.h"

namespace game
{

class StateGame : public BaseGameState
{
public:
    StateGame(Game * game);

    bool Save(sgl::utilities::BinaryFile & bf) const override;

private:
    void OnActive() override;
    void OnInactive() override;
};

} // namespace game
