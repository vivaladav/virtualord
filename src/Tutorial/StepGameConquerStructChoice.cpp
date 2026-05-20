#include "Tutorial/StepGameConquerStructChoice.h"

#include "Game.h"
#include "GameConstants.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerStructChoice::StepGameConquerStructChoice(const Game * game, const GameObject * unit,
                                                         const GameObject * struct1,
                                                         const GameObject * struct2,
                                                         const IsoMap * isoMap)
    : TutorialInfoStep(550, 260)
    , mFocusArea1(new FocusArea)
    , mFocusArea2(new FocusArea)
    , mStruct1(struct1)
    , mStruct2(struct2)
    , mUnit(unit)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea1->SetCornersColorAction();
    mFocusArea1->SetVisible(false);
    mFocusArea2->SetCornersColorAction();
    mFocusArea2->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1000, 200);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT_4"), 7.f, true, false);
    info->AddActionEntry(sm->GetCString("TUT_GAME_CONQUER_STRUCT_5"), 0.f, false, false,
                        [this, struct1, struct2, isoMap, game]
                        {
                            // STRUCTURE 1 - FOCUS
                            const auto isoObj1 = mStruct1->GetIsoObject();
                            const int objX1 = isoObj1->GetX();
                            const int objY1 = isoObj1->GetY();
                            const int objW1 = isoObj1->GetWidth();
                            const int objH1 = isoObj1->GetHeight();

                            mFocusArea1->SetWorldArea(objX1, objY1, objW1, objH1);
                            mFocusArea1->SetBlinking(true);
                            mFocusArea1->SetVisible(true);

                            // STRUCTURE 2 - FOCUS
                            const auto isoObj2 = mStruct2->GetIsoObject();
                            const int objX2 = isoObj2->GetX();
                            const int objY2 = isoObj2->GetY();
                            const int objW2 = isoObj2->GetWidth();
                            const int objH2 = isoObj2->GetHeight();

                            mFocusArea2->SetWorldArea(objX2, objY2, objW2, objH2);
                            mFocusArea2->SetBlinking(true);
                            mFocusArea2->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX1, objY1, objW1, objH1);
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->SetClickableCells(isoMap, struct1->GetRow1(), struct1->GetCol1(),
                                                  struct1->GetRow0(), struct1->GetCol0());

                            // TODO handle secondary area/cells in ClickFilter
                        });
}

StepGameConquerStructChoice::~StepGameConquerStructChoice()
{
    delete mFocusArea1;
    delete mFocusArea2;
}

void StepGameConquerStructChoice::Update(float)
{
    if(mStruct1->GetFaction() != NO_FACTION || mStruct2->GetFaction() != NO_FACTION)
        SetDone();
    else if(mUnit != nullptr && mUnit->GetCurrentAction() == CONQUER_STRUCTURE)
    {
        mFocusArea1->SetVisible(false);
        mFocusArea2->SetVisible(false);

        // hide info panel while conquest is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
