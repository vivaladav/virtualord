#pragma once

#include "BaseGameState.h"

namespace game
{

class StatePlanetMap : public BaseGameState
{
public:
    StatePlanetMap(Game * game);

    bool Save(sgl::utilities::BinaryFile & bf) const override;

private:
    void OnActive() override;
    void OnInactive() override;
};

} // namespace game
