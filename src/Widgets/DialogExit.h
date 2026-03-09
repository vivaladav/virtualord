#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }
    namespace sgui { class AbstractButton; }
}

namespace game
{

class Game;
class Screen;

class DialogExit : public sgl::sgui::Widget
{
public:
    enum DialogButtons : unsigned int
    {
        BTN_MAIN_MENU = 0x01,
        BTN_PLANET_MAP = 0x02,
        BTN_QUIT_TUTORIAL = 0x04,
        BTN_SETTINGS = 0x08,

        BUTTONS_EXIT = BTN_MAIN_MENU | BTN_PLANET_MAP | BTN_SETTINGS,
        BUTTONS_TUTORIAL = BTN_QUIT_TUTORIAL
    };

public:
    DialogExit(DialogButtons buttons, Game * game, Screen * screen);

    void SetFunctionOnShowingDialogSettings(const std::function<void()> & f);
    void SetFunctionOnHidingDialogSettings(const std::function<void()> & f);

    void SetFunctionOnClose(const std::function<void()> & f);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBgTop = nullptr;
    sgl::graphic::Image * mBgMid = nullptr;
    sgl::graphic::Image * mBgBot = nullptr;

    sgl::sgui::AbstractButton * mButtonClose = nullptr;

    std::function<void()> mOnShowingSettings;
    std::function<void()> mOnHidingSettings;
};

} // namespace game
