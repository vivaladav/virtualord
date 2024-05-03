#include "Tutorial/StepGameUnitConquerCellsIcon.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepGameUnitConquerCellsIcon::StepGameUnitConquerCellsIcon(PanelObjectActions * panel)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(450, 200);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 700);

    mInfo->AddInfoEntry("Click this button to set your unit's action to CELL CONQUEST.",
                        colorTutorialTextAction, 0.f, false, false, [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButton(PanelObjectActions::BTN_CONQUER_CELL);

                            const int padding = 10;
                            const int fX = panel->GetX() + btn->GetX() - padding;
                            const int fY = panel->GetY() + btn->GetY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);

                        });

    panel->SetButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, [this]
    {
        SetDone();
    });
}

StepGameUnitConquerCellsIcon::~StepGameUnitConquerCellsIcon()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameUnitConquerCellsIcon::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
