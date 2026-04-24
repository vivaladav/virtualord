#include "Tutorial/StepGameConquerStructSimple.h"

#include "GameConstants.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerStructSimple::StepGameConquerStructSimple(const Unit * unit,
                                                         const GameObject * energyGen,
                                                         const IsoMap * isoMap,
                                                         const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new FocusArea)
    , mStruct(energyGen)
    , mUnit(unit)
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

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT"),
                       TutorialConstants::colorTextAction, 0.f, false, false,
                       [this, objX, objY, objW, objH, energyGen, isoMap]
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
    else if(mUnit != nullptr &&
            (mUnit->GetCurrentAction() == CONQUER_STRUCTURE || mUnit->GetCurrentAction() == MOVE))
    {
        mFocusArea->SetVisible(false);

        // hide info panel while conquest is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
