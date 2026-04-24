#include "Tutorial/StepGameBuildTowerEnd.h"

#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBuildTowerEnd::StepGameBuildTowerEnd(const IsoMap * isoMap, const Player * p,
                                             const Cell2D & cell)
    : TutorialInfoStep(600, 200)
    , mFocusArea(new FocusArea)
    , mTarget(cell)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(900, 250);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BUILD_DTOWER_5"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, isoMap, p]
                       {
                           const auto pos = isoMap->GetCellPosition(mTarget.row, mTarget.col);

                           // FOCUS
                           const int marginW = 5;
                           const int marginH = 10;
                           const int objX = pos.x - marginW;
                           const int objY = pos.y - marginH;
                           const int objW = isoMap->GetTileWidth() + (2 * marginW);
                           const int objH = isoMap->GetTileHeight() + (2 * marginH);

                           mFocusArea->SetWorldArea(objX, objY, objW, objH);
                           mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           auto cf = GetClickFilter();
                           cf->SetWorldClickableArea(objX, objY, objW, objH);
                           cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);

                           mUnit = p->GetUnit(0);
                       });
}

StepGameBuildTowerEnd::~StepGameBuildTowerEnd()
{
    delete mFocusArea;
}

void StepGameBuildTowerEnd::Update(float)
{
    if(mUnit == nullptr)
        return ;

    if(mBuildStarted)
    {
        if(mUnit->GetCurrentAction() == IDLE)
            SetDone();
    }
    else
    {
        if(mUnit->GetCurrentAction() == GameObjectActionType::BUILD_STRUCTURE)
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
