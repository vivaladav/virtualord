#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace sgl
{
    namespace graphic { class Camera; }
}

namespace game
{

class CameraMapController;

class StepGameTestCameraFocus : public TutorialInfoStep
{
public:
    StepGameTestCameraFocus(const char * text, const sgl::graphic::Camera * c,
                            int tlX, int tlY, int brX, int brY);

    void Update(float delta) override;

private:
    const sgl::graphic::Camera * mCamera = nullptr;

    int mTLx;
    int mTLy;
    int mBRx;
    int mBRy;
};

} // namespace game
