#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl  { namespace graphic  { class Renderable; } }

namespace game
{

enum PlayerFaction : unsigned int;

class CountdownLabel : public sgl::sgui::Widget
{
public:
    CountdownLabel(PlayerFaction faction, unsigned int turns, sgl::sgui::Widget * parent = nullptr);

    void AddPlayedTurn();

private:
    void UpdateDigits();

    void HandlePositionChanged() override;

    void SetPositions();

private:
    unsigned int mTurns = 0;

    PlayerFaction mFaction;

    sgl::graphic::Renderable * mTxt = nullptr;
};

} // namespace game
