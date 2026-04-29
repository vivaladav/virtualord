#include "Tutorial/StepGameMoveCameraOverObject.h"

#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/PanelClickFilter.h"

#include <sgl/graphic/Camera.h>

#include <cmath>

namespace game
{

StepGameMoveCameraOverObject::StepGameMoveCameraOverObject(const GameObject * obj, float speed)
    : mCamera(sgl::graphic::Camera::GetDefaultCamera())
    , mSpeed(speed)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // DEFINE MOVE
    const IsoObject * isoObj = obj->GetIsoObject();
    const int cX = isoObj->GetX() + isoObj->GetWidth() / 2;
    const int cY = isoObj->GetY() + isoObj->GetHeight() / 2;

    const int deltaX = cX - (mCamera->GetWidth() / 2) - mCamera->GetX();
    const int deltaY = cY - (mCamera->GetHeight() / 2) - mCamera->GetY();

    Move(deltaX, deltaY);
}

StepGameMoveCameraOverObject::~StepGameMoveCameraOverObject()
{
    delete mClickFilter;
}

void StepGameMoveCameraOverObject::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);
}

void StepGameMoveCameraOverObject::Update(float delta)
{
    int todo = 2;

    if(mMovingX)
    {
        mCamera->MoveX(mMovX * delta);

        if(mMovX > 0 && mCamera->GetX() >= mTargetX)
        {
            mCamera->SetX(mTargetX);

            --todo;
            mMovingX = false;
        }
        else if(mMovX < 0 && mCamera->GetX() <= mTargetX)
        {
            mCamera->SetX(mTargetX);

            --todo;
            mMovingX = false;
        }
    }
    else
        --todo;

    if(mMovingY)
    {
        mCamera->MoveY(mMovY * delta);

        if(mMovY > 0 && mCamera->GetY() >= mTargetY)
        {
            mCamera->SetY(mTargetY);

            --todo;
            mMovingY = false;
        }
        else if(mMovY < 0 && mCamera->GetY() <= mTargetY)
        {
            mCamera->SetY(mTargetY);

            --todo;
            mMovingY = false;
        }
    }
    else
        --todo;

    if(0 == todo)
        SetDone();
}

void StepGameMoveCameraOverObject::Move(int deltaX, int deltaY)
{
    mTargetX = mCamera->GetX() + deltaX;
    mTargetY = mCamera->GetY() + deltaY;

    float multX = (0 == deltaX || 0 == deltaY) ?
                  1.f : std::fabs(static_cast<float>(deltaX) / static_cast<float>(deltaY));

    // X
    mMovingX = true;

    if(deltaX > 0)
        mMovX = mSpeed * multX;
    else if(deltaX < 0)
        mMovX = -mSpeed * multX;
    else
    {
        mMovX = 0.f;
        mMovingX = false;
    }

    // Y
    mMovingY = true;

    if(deltaY > 0)
        mMovY = mSpeed;
    else if(deltaY < 0)
        mMovY = -mSpeed;
    else
    {
        mMovY = 0.f;
        mMovingY = false;
    }
}

} // namespace game
