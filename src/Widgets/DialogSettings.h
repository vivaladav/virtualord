#pragma once

#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringsChangeListener.h>

#include <functional>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class AbstractButtonsGroup;
        class ComboBox;
        class ImageButton;
        class Label;
    }
}

namespace game
{

class Game;

class DialogSettings : public sgl::sgui::Widget, public sgl::utilities::StringsChangeListener
{
public:
    DialogSettings(Game * game);
    ~DialogSettings();

    void AddOnCloseClickedFunction(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void SetPositions();

    void CreatePanelGame(sgl::sgui::Widget * parent);
    void CreatePanelAudio(sgl::sgui::Widget * parent);
    void CreatePanelVideo(sgl::sgui::Widget * parent);
    void CreatePanelControls(sgl::sgui::Widget * parent);

    void UpdateCurrentResolution();

    void OnStringsChanged() override;

private:
    enum Panel : unsigned int
    {
        GAME,
        AUDIO,
        VIDEO,
        CONTROLS,

        NUM_PANELS
    };

private:
    std::vector<sgl::sgui::AbstractButton *> mButtonsTabs;
    std::vector<sgl::sgui::Label *> mHeadersGame;
    std::vector<sgl::sgui::Label *> mHeadersAudio;
    std::vector<sgl::sgui::Label *> mHeadersVideo;

    Game * mGame = nullptr;

    sgl::sgui::Label * mTitle = nullptr;

    sgl::sgui::AbstractButtonsGroup * mGroupButtons = nullptr;

    sgl::sgui::Widget * mPanels[NUM_PANELS];

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::ImageButton * mButtonClose = nullptr;

    sgl::sgui::ComboBox * mComboLang = nullptr;
    sgl::sgui::ComboBox * mComboRes = nullptr;
    sgl::sgui::ComboBox * mComboVMode = nullptr;

    sgl::utilities::StringManager * mSM = nullptr;
};

} // namespace game
