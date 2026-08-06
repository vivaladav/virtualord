#include "Tutorial/StepGameOpenLootbox.h"

#include "Game.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameOpenLootbox::StepGameOpenLootbox(const Game * game, const Unit * unit,
                                         const GameObject * lootbox, const IsoMap * isoMap,
                                         const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new FocusArea)
    , mIsoFocusArea(new IsoFocusArea(isoMap))
    , mLootbox(lootbox)
    , mUnit(unit)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS AREAS
    const auto isoObj = lootbox->GetIsoObject();
    const int padding = 5;
    const int objX = isoObj->GetX() - padding;
    const int objY = isoObj->GetY() - padding;
    const int objW = isoObj->GetWidth() + (padding * 2);
    const int objH = isoObj->GetHeight() + (padding * 2);

    mFocusArea->SetWorldArea(objX, objY, objW, objH, false);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    mIsoFocusArea->SetCell(mLootbox->GetRow0(), mLootbox->GetCol0());
    mIsoFocusArea->SetCornersColorAction();
    mIsoFocusArea->SetBlinking(true);
    mIsoFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_LOOTBOX_1"), 7.f, true, false,
                       [this]
                       {
                            mFocusArea->SetVisible(true);
                       });

    info->AddActionEntry(sm->GetCString("TUT_GAME_LOOTBOX_2"), 0.f, false, false,
                        [this, isoMap, game]
                        {
                            // FOCUS AREAS
                            mFocusArea->SetVisible(false);

                            mIsoFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->SetClickableCell(isoMap, mLootbox->GetRow0(), mLootbox->GetCol0());
                        });
}

StepGameOpenLootbox::~StepGameOpenLootbox()
{
    delete mFocusArea;
    delete mIsoFocusArea;
}

void StepGameOpenLootbox::Update(float)
{
    if(mUnit != nullptr && mUnit->GetCurrentAction() == OPEN_LOOTBOX)
        SetDone();
}

} // namespace game
