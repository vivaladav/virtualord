#include "Tutorial/StepGameTestCameraFocus.h"

#include <sgl/graphic/Camera.h>

namespace game
{

StepGameTestCameraFocus::StepGameTestCameraFocus(const sgl::graphic::Camera *c, int tlX, int tlY, int brX, int brY)
    : mCamera(c)
    , mTLx(tlX)
    , mTLy(tlY)
    , mBRx(brX)
    , mBRy(brY)
{
}

void StepGameTestCameraFocus::Update(float)
{
    if(mCamera->IsRectangleInside(mTLx, mTLy, mBRx, mBRy))
        SetDone();
}

} // namespace game
