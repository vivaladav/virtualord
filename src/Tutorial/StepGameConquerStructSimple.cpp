#include "Tutorial/StepGameConquerStructSimple.h"

#include "Game.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerStructSimple::StepGameConquerStructSimple(const Game * game, const Unit * unit,
                                                         const GameObject * obj,
                                                         const IsoMap * isoMap,
                                                         const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mStruct(obj)
    , mUnit(unit)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT"), 0.f, false, false,
                        [this, isoMap, game]
                        {
                            // FOCUS
                            mFocusArea->SetCellArea(mStruct->GetRow0(), mStruct->GetCol0(),
                                                    mStruct->GetRow1(), mStruct->GetCol1());
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const auto isoObj = mStruct->GetIsoObject();
                            const int objX = isoObj->GetX();
                            const int objY = isoObj->GetY();
                            const int objW = isoObj->GetWidth();
                            const int objH = isoObj->GetHeight();

                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX, objY, objW, objH);
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->SetClickableCells(isoMap, mStruct->GetRow1(), mStruct->GetCol1(),
                                                  mStruct->GetRow0(), mStruct->GetCol0());
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
        GetPanelInfo()->SetVisible(false);
    }
}

} // namespace game
