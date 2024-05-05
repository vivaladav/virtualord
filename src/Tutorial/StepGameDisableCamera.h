#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class CameraMapController;

class StepGameDisableCamera : public TutorialStep
{
public:
    StepGameDisableCamera(CameraMapController * c);

    void OnStart() override;

private:
    CameraMapController * mController = nullptr;
};

} // namespace game
