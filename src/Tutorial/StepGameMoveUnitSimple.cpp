#include "Tutorial/StepGameMoveUnitSimple.h"

#include "Game.h"
#include "IsoMap.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMoveUnitSimple::StepGameMoveUnitSimple(const Game * game, const Unit * unit,
                                               const IsoMap * isoMap, const Cell2D & target,
                                               const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mUnit(unit)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_MOVE_UNIT"), 0.f, false, false, [this, isoMap, game]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mTarget.row, mTarget.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                            cf->SetButtonToAllow(game->GetButtonAction());
                        });
}

StepGameMoveUnitSimple::~StepGameMoveUnitSimple()
{
    delete mFocusArea;
}

void StepGameMoveUnitSimple::Update(float)
{
    if(mUnit->GetRow0() == mTarget.row && mUnit->GetCol0() == mTarget.col)
        SetDone();
    // hide focus area when move starts
    else if(mUnit->GetCurrentAction() == GameObjectActionType::MOVE)
    {
        mFocusArea->SetBlinking(false);
        mFocusArea->SetVisible(false);

        // hide info panel while move is in progress
        auto info = GetPanelInfo();
        info->SetVisible(false);
    }
}

} // namespace game
