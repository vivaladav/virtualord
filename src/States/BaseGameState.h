#pragma once

#include <sgl/utilities/State.h>

namespace game
{

class Game;
class Screen;

class BaseGameState : public sgl::utilities::State
{
public:
    BaseGameState(unsigned int stateId, Game * game);
    ~BaseGameState();

    void Update(float delta) override;

    void Render();

protected:
    Game * mGame = nullptr;

    Screen * mScreen = nullptr;
};

} // namespace game
