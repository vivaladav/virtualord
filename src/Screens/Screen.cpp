#include "Screens/Screen.h"

#include "Game.h"
#include "Screens/SharedScreenListener.h"
#include "Widgets/DialogSettings.h"
#include "Widgets/DisappearingLabel.h"
#include "Widgets/ScreenOverlay.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/sgui/Stage.h>
#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

Screen::Screen(Game * game)
    : mGame(game)
    , mSharedListener(new SharedScreenListener(game, this))
{
    mGame->AddKeyboardListener(mSharedListener);

    auto cam = sgl::graphic::Camera::GetDefaultCamera();
    cam->ClearFunctionOnMove();
    cam->ResetPosition();
}

Screen::~Screen()
{
    delete mSharedListener;
}

bool Screen::Save(sgl::utilities::BinaryFile &) const
{
    return true;
}

DialogSettings * Screen::ShowDialogSettings()
{
    using namespace sgl;

    ShowScreenOverlay();

    // DIALOG
    const int screenW = graphic::Renderer::Instance()->GetWidth();
    const int screenH = graphic::Renderer::Instance()->GetHeight();

    mSettings = new DialogSettings(mGame);
    mSettings->SetFocus();
    mSettings->SetPosition((screenW - mSettings->GetWidth()) / 2,
                           (screenH - mSettings->GetHeight()) / 2);

    mSettings->AddOnCloseClickedFunction([this]
    {
        mSettings->DeleteLater();
        mSettings = nullptr;

        HideScreenOverlay();
    });

    return mSettings;
}

void Screen::ShowLabelGameSaved()
{
    using namespace sgl;

    const int screenW = graphic::Renderer::Instance()->GetWidth();
    const int screenH = graphic::Renderer::Instance()->GetHeight();
    const int textSize = 26;
    const float time = 2.f;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    auto font = fm->GetFont(WidgetsConstants::FontFileText, textSize, graphic::Font::NORMAL);

    auto label = new DisappearingLabel(font, sm->GetCString("GAME_SAVED"), time);

    const int marginR = 100;
    const int marginB = 100;
    const int x = (screenW) - label->GetWidth() - marginR;
    const int y = screenH - label->GetHeight() - marginB;
    label->SetPosition(x, y);
}

void Screen::ShowScreenOverlay()
{
    if(mOverlay != nullptr)
        return ;

    mOverlay = new ScreenOverlay;

    sgl::sgui::Stage::Instance()->MoveChildToFront(mOverlay);
}

void Screen::HideScreenOverlay()
{
    delete mOverlay;
    mOverlay = nullptr;
}

} // namespace game
