#include "Tutorial/StepGameBuildTowerEnd.h"

#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBuildTowerEnd::StepGameBuildTowerEnd(const IsoMap * isoMap, const Unit * unit,
                                             const Cell2D & cell, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mUnit(unit)
    , mTarget(cell)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_DTOWER_5"), 0.f, false, false,
                         [this, isoMap, unit]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mTarget.row, mTarget.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const int marginW = 5;
                            const int marginH = 10;
                            const auto pos = isoMap->GetCellPosition(mTarget.row, mTarget.col);
                            const int objX = pos.x - marginW;
                            const int objY = pos.y - marginH;
                            const int objW = isoMap->GetTileWidth() + (2 * marginW);
                            const int objH = isoMap->GetTileHeight() + (2 * marginH);

                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX, objY, objW, objH);
                            cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                       });
}

StepGameBuildTowerEnd::~StepGameBuildTowerEnd()
{
    delete mFocusArea;
}

void StepGameBuildTowerEnd::Update(float)
{
    if(mBuildStarted)
    {
        if(mUnit->GetCurrentAction() == IDLE)
            SetDone();
    }
    else
    {
        const GameObjectActionType currAct = mUnit->GetCurrentAction();

        if(currAct == BUILD_STRUCTURE || currAct == MOVE
            )
        {
            mFocusArea->SetBlinking(false);
            mFocusArea->SetVisible(false);

            mBuildStarted = true;

            // hide info panel while construction is in progress
            auto info = GetPanelInfo();
            info->SetVisible(false);
        }
    }
}

} // namespace game
