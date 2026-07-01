#include "Tutorial/StepGameEnableCamera.h"

#include "CameraMapController.h"

namespace game
{

StepGameEnableCamera::StepGameEnableCamera(CameraMapController * c)
    : mController(c)
{
}

void StepGameEnableCamera::OnStart()
{
    TutorialStep::OnStart();

    mController->SetEnabled(true);

    SetDone();
}

} // namespace game
