#include "Tutorial/StepGameUnitAttackFree.h"

#include "Game.h"
#include "GameMap.h"
#include "IsoMap.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameUnitAttackFree::StepGameUnitAttackFree(const Game * game, const Unit * unit,
                                               const GameObject * target, const IsoMap * isoMap,
                                               const GameMap * gm, const char * text)
    : TutorialInfoStep(500, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mGameMap(gm)
    , mUnit(unit)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->AddActionEntry(sm->GetCString(text), 0.f, false, false, [this, isoMap, game]
                        {
                            // FOCUS
                            mFocusArea->SetCellArea(mTarget->GetRow0(), mTarget->GetCol0(),
                                                    mTarget->GetRow1(), mTarget->GetCol1());
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->ExpandClickableArea();
                        });

    if(mUnit == nullptr || mTarget == nullptr || !mGameMap->HasObject(mTarget))
        SetDone();
}

StepGameUnitAttackFree::~StepGameUnitAttackFree()
{
    delete mFocusArea;
}

void StepGameUnitAttackFree::Update(float)
{
    if(!mGameMap->HasObject(mTarget) || mUnit->GetCurrentAction() == ACT_ATTACK)
        SetDone();
}

} // namespace game
