#include "Tutorial/StepGameTestCameraFocus.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/graphic/Camera.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameTestCameraFocus::StepGameTestCameraFocus(const char * text, const sgl::graphic::Camera * c,
                                                 int tlX, int tlY, int brX, int brY)
    : TutorialInfoStep(500, 120)
    , mCamera(c)
    , mTLx(tlX)
    , mTLy(tlY)
    , mBRx(brX)
    , mBRy(brY)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(700, 200);

    info->AddInfoEntry(sm->GetCString(text), 0.f, false, false);
}

void StepGameTestCameraFocus::Update(float)
{
    if(mCamera->IsRectangleInside(mTLx, mTLy, mBRx, mBRy))
        SetDone();
}

} // namespace game
