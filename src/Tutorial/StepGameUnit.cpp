#include "Tutorial/StepGameUnit.h"

#include "Game.h"
#include "IsoObject.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameUnit::StepGameUnit(const Game * game, const IsoMap * im, const Unit * unit)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new IsoFocusArea(im))
    , mUnit(unit)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1300, 450);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UNIT_1"), 0.f, false, false, [this, game]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mUnit->GetRow0(), mUnit->GetCol0());
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const auto isoObj = mUnit->GetIsoObject();
                            const int objX = isoObj->GetX();
                            const int objY = isoObj->GetY();
                            const int objW = isoObj->GetWidth();
                            const int objH = isoObj->GetHeight();

                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX, objY, objW, objH);
                            cf->SetButtonToAllow(game->GetButtonSelect());
                        });
}

StepGameUnit::~StepGameUnit()
{
    delete mFocusArea;
}

void StepGameUnit::Update(float)
{
    if(mUnit != nullptr && mUnit->IsSelected())
        SetDone();
}

} // namespace game
