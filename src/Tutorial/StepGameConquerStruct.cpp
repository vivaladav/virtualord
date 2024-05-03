#include "Tutorial/StepGameConquerStruct.h"

#include "GameConstants.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>

namespace game
{

StepGameConquerStruct::StepGameConquerStruct(const GameObject * energyGen, const IsoMap * isoMap)
    : mEnergyGen(energyGen)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto isoObj = mEnergyGen->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea = new FocusArea;
    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColor(colorTutorialFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(500, 300);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 200);

    mInfo->AddInfoEntry("When your unit action is set to MOVE you can also conquer structures.", colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("For example we can conquer this ENERGY GENERATOR.", colorTutorialText, 6.f, true, false,
                        [this]
                        {
                            mFocusArea->SetVisible(true);
                        });
    mInfo->AddInfoEntry("Click on the generator with the RIGHT MOUSE BUTTON to start the conquest.",
                        colorTutorialTextAction, 0.f, false, false, [this, objX, objY, objW, objH, energyGen, isoMap]
                        {
                            // FOCUS
                            mFocusArea->SetCornersColor(colorTutorialFocusAction);

                            // CLICK FILTER
                            mClickFilter->SetWorldClickableArea(objX, objY, objW, objH);
                            mClickFilter->SetButtonToExclude(sgl::core::MouseEvent::BUTTON_LEFT);
                            mClickFilter->SetClickableCells(isoMap, energyGen->GetRow1(), energyGen->GetCol1(),
                                                            energyGen->GetRow0(), energyGen->GetCol0());
                        });
}

StepGameConquerStruct::~StepGameConquerStruct()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameConquerStruct::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void StepGameConquerStruct::Update(float)
{
    if(mEnergyGen->GetFaction() != NO_FACTION)
        SetDone();
}

} // namespace game
