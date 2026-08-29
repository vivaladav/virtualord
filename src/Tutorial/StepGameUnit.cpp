#include "Tutorial/StepGameUnit.h"

#include "Game.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameUnit::StepGameUnit(const Game * game, const IsoMap * im, const Unit * unit,
                           const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new IsoFocusArea(im))
    , mUnit(unit)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UNIT_1"), 0.f, false, false, [this, game]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mUnit->GetRow0(), mUnit->GetCol0());
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const int objX = mUnit->GetX();
                            const int objY = mUnit->GetY();
                            const int objW = mUnit->GetWidth();
                            const int objH = mUnit->GetHeight();

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
