#pragma once

#include <sgl/sgui/Widget.h>


namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class AbstractButtonsGroup;
        class Label;
    }
}

namespace game
{

class Game;

enum PlayerFaction : unsigned int;

class DialogFactionSelection : public sgl::sgui::Widget
{
public:
    DialogFactionSelection(Game * game);

private:
    sgl::sgui::Widget * CreateFactionRow(PlayerFaction faction, const std::vector<int> & attributes);

    void HandlePositionChanged() override;

    void SetPositions();

private:

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mButtonClose = nullptr;
    sgl::sgui::AbstractButton * mButtonSel = nullptr;

    sgl::sgui::AbstractButtonsGroup * mButtonsFact = nullptr;

    Game * mGame = nullptr;

    PlayerFaction mFaction;
};

} // namespace game
