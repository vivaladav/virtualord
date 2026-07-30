#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class AbstractButton; }
}

namespace game
{

class Game;

class DialogSave : public sgl::sgui::Widget
{
public:
    DialogSave(Game * game);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void SetPositions();

private:
    sgl::graphic::Image * mBgTop = nullptr;
    sgl::graphic::Image * mBgMid = nullptr;
    sgl::graphic::Image * mBgBot = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
};

} // namespace game
