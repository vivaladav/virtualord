#include "Tutorial/StepGameMoveUnitToArea.h"

#include "IsoMap.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMoveUnitToArea::StepGameMoveUnitToArea(const Unit * unit, const IsoMap * isoMap,
                                               const Cell2D & areaTL, const Cell2D & areaBR,
                                               const Cell2D & target, const char * intro,
                                               const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 250)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mUnit(unit)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCellArea(areaBR.row, areaBR.col, areaTL.row, areaTL.col, false);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString(intro), 10.f, true, false, [this]
                        {
                            mFocusArea->SetVisible(true);
                            mFocusArea->SetBlinking(true);
                        });
    info->AddActionEntry(sm->GetCString("TUT_GAME_MOVE_UNIT"), 0.f, false, false, [this, isoMap]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mTarget.row, mTarget.col);
                            mFocusArea->SetCornersColorAction();

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                        });
}

StepGameMoveUnitToArea::~StepGameMoveUnitToArea()
{
    delete mFocusArea;
}

void StepGameMoveUnitToArea::Update(float)
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
