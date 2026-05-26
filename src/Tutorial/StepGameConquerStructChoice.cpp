#include "Tutorial/StepGameConquerStructChoice.h"

#include "Game.h"
#include "GameConstants.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
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
    : TutorialInfoStep(550, 200)
    , mFocusArea1(new IsoFocusArea(isoMap))
    , mFocusArea2(new IsoFocusArea(isoMap))
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
                            mFocusArea1->SetCellArea(struct1->GetRow0(), struct1->GetCol0(),
                                                     struct1->GetRow1(), struct1->GetCol1());
                            mFocusArea1->SetBlinking(true);
                            mFocusArea1->SetVisible(true);

                            // STRUCTURE 2 - FOCUS
                            mFocusArea2->SetCellArea(struct2->GetRow0(), struct2->GetCol0(),
                                                     struct2->GetRow1(), struct2->GetCol1());
                            mFocusArea2->SetBlinking(true);
                            mFocusArea2->SetVisible(true);

                            // CLICK FILTER
                            const IsoObject * obj1 = mStruct1->GetIsoObject();
                            const IsoObject * obj2 = mStruct2->GetIsoObject();
                            const int tlX1 = obj1->GetX();
                            const int tlX2 = obj2->GetX();
                            const int tlY1 = obj1->GetY();
                            const int tlY2 = obj2->GetY();
                            const int brX1 = tlX1 + obj1->GetWidth();
                            const int brX2 = tlX2 + obj2->GetWidth();
                            const int brY1 = tlY1 + obj1->GetHeight();
                            const int brY2 = tlY2 + obj2->GetHeight();
                            const int minX = tlX1 < tlX2 ? tlX1 : tlX2;
                            const int minY = tlY1 < tlY2 ? tlY1 : tlY2;
                            const int maxX = brX1 > brX2 ? brX1 : brX2;
                            const int maxY = brY1 > brY2 ? brY1 : brY2;
                            const int w = std::abs(maxX - minX);
                            const int h = std::abs(maxY - minX);

                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(minX, minY, w, h);
                            cf->SetButtonToAllow(game->GetButtonAction());

                            cf->AddClickableCells(isoMap, struct1->GetRow1(), struct1->GetCol1(),
                                                  struct1->GetRow0(), struct1->GetCol0());
                            cf->AddClickableCells(isoMap, struct2->GetRow1(), struct2->GetCol1(),
                                                  struct2->GetRow0(), struct2->GetCol0());
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
    else if(mUnit != nullptr && mUnit->GetCurrentAction() == MOVE)
    {
        mFocusArea1->SetVisible(false);
        mFocusArea2->SetVisible(false);

        // hide info panel while conquest is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
