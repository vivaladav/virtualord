#include "Tutorial/StepGameUnitSelect.h"

#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameUnitSelect::StepGameUnitSelect(const Player * p)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(500, 200);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 350);

    mInfo->AddInfoEntry("Okay, let's do it!", colorTutorialText, 4.f, true, false);
    mInfo->AddInfoEntry("Select your unit with the LEFT MOUSE BUTTON.",
                        colorTutorialTextAction, 0.f, false, false, [this, p]
                        {
                            // FOCUS
                            mUnit = p->GetUnit(0);
                            auto isoObj = mUnit->GetIsoObject();
                            const int objX = isoObj->GetX();
                            const int objY = isoObj->GetY();
                            const int objW = isoObj->GetWidth();
                            const int objH = isoObj->GetHeight();

                            mFocusArea->SetWorldArea(objX, objY, objW, objH);
                            mFocusArea->SetCornersColor(colorTutorialFocusAction);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            mClickFilter->SetWorldClickableArea(objX, objY, objW, objH);
                        });
}

StepGameUnitSelect::~StepGameUnitSelect()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameUnitSelect::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void StepGameUnitSelect::Update(float)
{
    if(mUnit != nullptr && mUnit->IsSelected())
            SetDone();
}

} // namespace game
