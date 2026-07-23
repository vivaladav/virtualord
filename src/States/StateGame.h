#pragma once

#include "BaseGameState.h"

namespace sgl { namespace utilities { class BinaryFile; } }

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

private:
    sgl::utilities::BinaryFile * mBinFile = nullptr;
};

} // namespace game
