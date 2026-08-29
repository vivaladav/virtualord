#include "Tutorial/StepGameBase.h"

#include "Game.h"
#include "GameObjects/Base.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBase::StepGameBase(const Game * game, const IsoMap * im, const Base * b)
    : TutorialInfoStep(600, 250)
    , mFocusArea(new FocusArea)
    , mIsoFocusArea(new IsoFocusArea(im))
    , mBase(b)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS AREAS
    const int objX = mBase->GetX();
    const int objY = mBase->GetY();
    const int objW = mBase->GetWidth();
    const int objH = mBase->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH, false);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    mIsoFocusArea->SetCellArea(mBase->GetRow0(), mBase->GetCol0(), mBase->GetRow1(), mBase->GetCol1());
    mIsoFocusArea->SetVisible(false);

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

    info->SetFunctionOnFinished([this, game]
    {
        // FOCUS AREAS
        mFocusArea->SetVisible(false);

        mIsoFocusArea->SetCornersColorAction();
        mIsoFocusArea->SetBlinking(true);
        mIsoFocusArea->SetVisible(true);

        // CLICK FILTER
        const int objX = mBase->GetX();
        const int objY = mBase->GetY();
        const int objW = mBase->GetWidth();
        const int objH = mBase->GetHeight();

        auto cf = GetClickFilter();
        cf->SetWorldClickableArea(objX, objY, objW, objH);
        cf->SetButtonToAllow(game->GetButtonSelect());

        mCheckBaseSelected = true;
    });
}

StepGameBase::~StepGameBase()
{
    delete mFocusArea;
    delete mIsoFocusArea;
}

void StepGameBase::OnStart()
{
    TutorialInfoStep::OnStart();

    // FOCUS AREA
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
