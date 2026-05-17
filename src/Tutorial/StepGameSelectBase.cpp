#include "Tutorial/StepGameSelectBase.h"

#include "IsoObject.h"
#include "GameObjects/Base.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameSelectBase::StepGameSelectBase(const Base * b)
    : TutorialInfoStep(500, 150)
    , mFocusArea(new FocusArea)
    , mBase(b)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(500, 200);

    info->AddActionEntry(sm->GetCString("TUT_GAME_BASE_4"), 0.f, false, false, [this]
                        {
                            const auto isoObj = mBase->GetIsoObject();
                            const int objX = isoObj->GetX();
                            const int objY = isoObj->GetY();
                            const int objW = isoObj->GetWidth();
                            const int objH = isoObj->GetHeight();

                            mFocusArea->SetWorldArea(objX, objY, objW, objH);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetVisible(true);

                            GetClickFilter()->SetWorldClickableArea(objX, objY, objW, objH);
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
