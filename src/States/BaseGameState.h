#pragma once

#include <sgl/utilities/State.h>

namespace sgl { namespace utilities { class BinaryFile; } }

namespace game
{

class Game;
class Screen;

class BaseGameState : public sgl::utilities::State
{
public:
    BaseGameState(unsigned int stateId, Game * game);
    ~BaseGameState();

    virtual bool Save(sgl::utilities::BinaryFile & bf) const;

    void Update(float delta) override;

    void Render();

protected:
    Game * mGame = nullptr;

    Screen * mScreen = nullptr;
};

} // namespace game
