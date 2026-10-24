#include "Tutorial/StepGameBuildStructArea.h"

#include "Game.h"
#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBuildStructArea::StepGameBuildStructArea(const Game * game, const IsoMap * isoMap,
                                                 const Cell2D & areaTL, const Cell2D & areaBR,
                                                 const char * text)
    : TutorialInfoStep(550, 130)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mPlayer(game->GetLocalPlayer())
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);
    mFocusArea->SetCellArea(areaBR.row, areaBR.col, areaTL.row, areaTL.col, false);
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);

    // CLICK FILTER
    auto cf = GetClickFilter();
    cf->SetButtonToAllow(game->GetButtonAction());
    cf->AddClickableCells(isoMap, areaTL.row, areaTL.col, areaBR.row, areaBR.col);

    // INFO
    if(text != nullptr)
        GetPanelInfo()->AddActionEntry(sm->GetCString(text), 0.f, false, false);
}

StepGameBuildStructArea::~StepGameBuildStructArea()
{
    delete mFocusArea;
}

void StepGameBuildStructArea::OnStart()
{
    TutorialInfoStep::OnStart();

    mFocusArea->SetVisible(true);
}

void StepGameBuildStructArea::Update(float)
{
    const GameObject * selObj = mPlayer->GetSelectedObject();

    if(selObj == nullptr)
        return ;

    if(mBuildStarted)
    {
       if(selObj->GetCurrentAction() == IDLE)
            SetDone();
    }
    else if(selObj->GetCurrentAction() == ACT_BUILD_STRUCTURE)
    {
        mFocusArea->SetBlinking(false);
        mFocusArea->SetVisible(false);

        mBuildStarted = true;
    }
}

} // namespace game
