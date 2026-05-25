#include "Tutorial/StepGameSelectBase.h"

#include "Game.h"
#include "IsoObject.h"
#include "GameObjects/Base.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameSelectBase::StepGameSelectBase(const Game * game, const IsoMap * im, const Base * b)
    : TutorialInfoStep(500, 150)
    , mFocusArea(new IsoFocusArea(im))
    , mBase(b)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(500, 200);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BASE_4"), 0.f, false, false, [this, game]
                        {
                            // FOCUS AREA
                            mFocusArea->SetCellArea(mBase->GetRow0(), mBase->GetCol0(),
                                                    mBase->GetRow1(), mBase->GetCol1());
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const auto isoObj = mBase->GetIsoObject();
                            const int objX = isoObj->GetX();
                            const int objY = isoObj->GetY();
                            const int objW = isoObj->GetWidth();
                            const int objH = isoObj->GetHeight();

                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX, objY, objW, objH);
                            cf->SetButtonToAllow(game->GetButtonSelect());
                        });
}

StepGameSelectBase::~StepGameSelectBase()
{
    delete mFocusArea;
}

void StepGameSelectBase::Update(float)
{
    if(mBase->IsSelected())
        SetDone();
}

} // namespace game
