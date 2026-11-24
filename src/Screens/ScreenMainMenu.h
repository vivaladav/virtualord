#pragma once

#include "Screen.h"

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class Widget; }
}

namespace game
{

class ButtonChangelog;
class DialogChangelog;

class ScreenMainMenu : public Screen
{
public:
    ScreenMainMenu(Game * game);
    ~ScreenMainMenu();

    void OnKeyUp(sgl::core::KeyboardEvent & event) override;

    void Update(float update) override;
    void Render() override;

private:
    void OnApplicationQuit(sgl::core::ApplicationEvent & event) override;

    void CreatePanelLoad();
    void CreateChangelog();

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Widget * mPanelButtons = nullptr;
    sgl::sgui::Widget * mPanelLoad = nullptr;

    ButtonChangelog * mButtonChangelog = nullptr;
    DialogChangelog * mDialogChangelog = nullptr;
};

} // namespace game
