#include "Tutorial/StepGameUnitConquerCellsIcon.h"

#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameUnitConquerCellsIcon::StepGameUnitConquerCellsIcon(PanelObjectActions * panel)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 650);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UNIT_CONQUE_CELLS_ICON_1"), 0.f, false, false,
                         [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButton(PanelObjectActions::BTN_CONQUER_CELL);

                            const int padding = 10;
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, [this]
    {
        SetDone();
    });
}

StepGameUnitConquerCellsIcon::~StepGameUnitConquerCellsIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, mClickId);

    delete mFocusArea;
}

} // namespace game
