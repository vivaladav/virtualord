#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class CameraMapController;

class StepGameEnableCamera : public TutorialStep
{
public:
    StepGameEnableCamera(CameraMapController * c);

    void OnStart() override;

private:
    CameraMapController * mController = nullptr;
};

} // namespace game
