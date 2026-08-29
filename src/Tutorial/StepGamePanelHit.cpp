#include "Tutorial/StepGamePanelHit.h"

#include "IsoMap.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGamePanelHit::StepGamePanelHit(const IsoMap * isoMap, const Cell2D & targetBR,
                                   const Cell2D & targetTL, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(500, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mTargetBR(targetBR)
    , mTargetTL(targetTL)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_ATTACK_5"), 12.f, true, false, [this, isoMap]
                        {
                            // FOCUS
                            mFocusArea->SetCellArea(mTargetBR.row, mTargetBR.col,
                                                    mTargetTL.row, mTargetTL.col, false);
                            mFocusArea->SetCornersColorElement();
                            mFocusArea->SetVisible(true);
                        });

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGamePanelHit::~StepGamePanelHit()
{
    delete mFocusArea;
}

void StepGamePanelHit::Update(float)
{
}

} // namespace game
