#pragma once

#include <sgl/sgui/Widget.h>


namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class ButtonsGroup;
        class ComboBox;
        class ImageButton;
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

    sgl::sgui::ImageButton * mButtonClose = nullptr;

    Game * mGame = nullptr;
};

} // namespace game
