#include "Tutorial/StepGameConquerStructSimple.h"

#include "GameConstants.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerStructSimple::StepGameConquerStructSimple(const Player * p,
                                                         const GameObject * energyGen,
                                                         const IsoMap * isoMap)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new FocusArea)
    , mStruct(energyGen)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const auto isoObj = mStruct->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColor(TutorialConstants::colorFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1300, 200);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT"),
                       TutorialConstants::colorTextAction, 0.f, false, false,
                       [this, objX, objY, objW, objH, energyGen, isoMap, p]
                       {
                           // FOCUS
                           mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           auto cf = GetClickFilter();
                           cf->SetWorldClickableArea(objX, objY, objW, objH);
                           cf->SetButtonToExclude(sgl::core::MouseEvent::BUTTON_LEFT);
                           cf->SetClickableCells(isoMap, energyGen->GetRow1(), energyGen->GetCol1(),
                                                 energyGen->GetRow0(), energyGen->GetCol0());

                           // re-allow unit to move and conquer
                           mUnit = p->GetUnit(0);
                           mUnit->SetActiveAction(MOVE);
                       });
}

StepGameConquerStructSimple::~StepGameConquerStructSimple()
{
    delete mFocusArea;
}

void StepGameConquerStructSimple::Update(float)
{
    if(mStruct->GetFaction() != NO_FACTION)
        SetDone();
    else if(mUnit != nullptr && mUnit->GetCurrentAction() == CONQUER_STRUCTURE)
        mFocusArea->SetVisible(false);
}

} // namespace game
