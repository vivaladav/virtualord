#pragma once

#include "Tutorial/TutorialStep.h"

namespace sgl
{
    namespace graphic { class Camera; }
}

namespace game
{

class CameraMapController;

class StepGameTestCameraFocus : public TutorialStep
{
public:
    StepGameTestCameraFocus(const sgl::graphic::Camera * c, int tlX, int tlY, int brX, int brY);

    void Update(float delta) override;

private:
    const sgl::graphic::Camera * mCamera = nullptr;

    int mTLx;
    int mTLy;
    int mBRx;
    int mBRy;
};

} // namespace game
