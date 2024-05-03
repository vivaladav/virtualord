#include "Tutorial/StepGameUnit.h"

#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameUnit::StepGameUnit(const Player * p)
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
    mInfo->SetPosition(1300, 450);

    mInfo->AddInfoEntry("Well done commander, this is your first unit!", colorTutorialText, 5.f, true, false);
    mInfo->AddInfoEntry("Now select it by clicking on its cell with the LEFT MOUSE BUTTON.",
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

StepGameUnit::~StepGameUnit()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameUnit::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void StepGameUnit::Update(float)
{
    if(mUnit != nullptr && mUnit->IsSelected())
        SetDone();
}

} // namespace game
