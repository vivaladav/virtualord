#include "Tutorial/StepGameBase.h"

#include "Game.h"
#include "IsoObject.h"
#include "GameObjects/Base.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBase::StepGameBase(const Game * game, const Base * b)
    : TutorialInfoStep(600, 250)
    , mFocusArea(new FocusArea)
    , mBase(b)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const auto isoObj = mBase->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1150, 400);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BASE_1"), 4.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_BASE_2"), 7.f, true, false);

    const char * buttonsStr[] =
    {
        "",
        "LMB_LO",
        "MMB_LO",
        "RMB_LO",
    };

    const std::string & strMouse = sm->GetString(buttonsStr[game->GetButtonSelect()]);
    const std::string str = sm->GetParametricString("TUT_GAME_BASE_3", strMouse);
    info->AddActionEntry(str.c_str(), 0.f, false, false);

    info->SetFunctionOnFinished([this, objX, objY, objW, objH, game]
    {
        mFocusArea->SetCornersColorAction();
        mFocusArea->SetBlinking(true);

        auto cf = GetClickFilter();
        cf->SetWorldClickableArea(objX, objY, objW, objH);
        cf->SetButtonToAllow(game->GetButtonSelect());

        mCheckBaseSelected = true;
    });
}

StepGameBase::~StepGameBase()
{
    delete mFocusArea;
}

void StepGameBase::OnStart()
{
    TutorialInfoStep::OnStart();

    // FOCUS
    mFocusArea->SetVisible(true);
}

void StepGameBase::Update(float)
{
    if(mCheckBaseSelected)
    {
        if(mBase->IsSelected())
            SetDone();
    }
}

} // namespace game
