#include "Tutorial/StepGameSetTarget.h"

#include "IsoMap.h"
#include "GameObjects/MiniUnitsGroup.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameSetTarget::StepGameSetTarget(const IsoMap * isoMap, const MiniUnitsGroup * group,
                                     const Cell2D & cell, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mGroup(group)
    , mTarget(cell)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_MU_4"), 0.f, false, false,
                         [this, isoMap]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mTarget.row, mTarget.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                       });
}

StepGameSetTarget::~StepGameSetTarget()
{
    delete mFocusArea;
}

void StepGameSetTarget::Update(float)
{

    if(mGroup->HasPathSet())
        SetDone();
}

} // namespace game
