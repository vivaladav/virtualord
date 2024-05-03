#include "Tutorial/StepGameBase.h"

#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameBase::StepGameBase(const GameObject * b)
    : mBase(b)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto isoObj = mBase->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea = new FocusArea;
    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColor(colorTutorialFocusElement);

    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(500, 250);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1150, 400);

    mInfo->AddInfoEntry("This is your base.", colorTutorialText, 3.f, true, false);
    mInfo->AddInfoEntry("You must protect it at all costs because if "
                        "destroyed you are defeated.", colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("Select it with the LEFT MOUSE BUTTON", colorTutorialTextAction, 0.f, false, false);

    mInfo->SetFunctionOnFinished([this, objX, objY, objW, objH]
    {
        mFocusArea->SetCornersColor(colorTutorialFocusAction);

        mClickFilter->SetWorldClickableArea(objX, objY, objW, objH);

        mCheckBaseSelected = true;
    });
}

StepGameBase::~StepGameBase()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameBase::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // FOCUS
    mFocusArea->SetVisible(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void StepGameBase::Update(float)
{
    if(mCheckBaseSelected)
    {
        if(mBase->IsSelected())
            SetDone();
    }
}

} // namespace game
