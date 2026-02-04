#pragma once

#include "Screens/Screen.h"

namespace game
{

class DummyScreen : public Screen
{
public:
    DummyScreen(Game * game);

    void Update(float delta) override;

    void Render() override;
};

} // namespace game
