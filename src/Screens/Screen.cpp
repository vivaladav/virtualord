#include "Screens/Screen.h"

#include "Game.h"
#include "Screens/SharedScreenListener.h"
#include "Widgets/DialogSettings.h"
#include "Widgets/ScreenOverlay.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/sgui/Stage.h>
#include <sgl/sgui/Widget.h>

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
