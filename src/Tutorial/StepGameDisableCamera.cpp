#include "Tutorial/StepGameDisableCamera.h"

#include "CameraMapController.h"

namespace game
{

StepGameDisableCamera::StepGameDisableCamera(CameraMapController * c)
    : mController(c)
{
}

void StepGameDisableCamera::OnStart()
{
    mController->SetEnabled(false);

    SetDone();
}

} // namespace game
