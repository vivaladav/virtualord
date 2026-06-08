#include "Tutorial/StepGameConquerStruct.h"

#include "Game.h"
#include "GameConstants.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerStruct::StepGameConquerStruct(const Game * game, const Player * p,
                                             const GameObject * energyGen, const IsoMap * isoMap)
    : TutorialInfoStep(550, 260)
    , mFocusArea(new FocusArea)
    , mIsoFocusArea(new IsoFocusArea(isoMap))
    , mEnergyGen(energyGen)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS AREAS
    const auto isoObj = mEnergyGen->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    mIsoFocusArea->SetCornersColorAction();
    mIsoFocusArea->SetVisible(false);
    mIsoFocusArea->SetCellArea(mEnergyGen->GetRow0(), mEnergyGen->GetCol0(),
                               mEnergyGen->GetRow1(), mEnergyGen->GetCol1());

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 200);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT_1"), 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT_2"), 7.f, true, false,
                       [this]
                       {
                           mFocusArea->SetVisible(true);
                       });
    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT_3"), 0.f, false, false,
                        [this, energyGen, isoMap, p, game]
                        {
                            // FOCUS
                            mFocusArea->SetVisible(false);

                            mIsoFocusArea->SetBlinking(true);
                            mIsoFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->AddClickableCells(isoMap, energyGen->GetRow1(), energyGen->GetCol1(),
                                                  energyGen->GetRow0(), energyGen->GetCol0());

                            // re-allow unit to move and conquer
                            mUnit = p->GetUnit(0);
                            mUnit->SetActiveAction(MOVE);
                        });
}

StepGameConquerStruct::~StepGameConquerStruct()
{
    delete mFocusArea;
    delete mIsoFocusArea;
}

void StepGameConquerStruct::Update(float)
{
    if(mEnergyGen->GetFaction() != NO_FACTION)
        SetDone();
    else if(mUnit != nullptr && mUnit->GetCurrentAction() == CONQUER_STRUCTURE)
    {
        mIsoFocusArea->SetVisible(false);

        // hide info panel while conquest is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
