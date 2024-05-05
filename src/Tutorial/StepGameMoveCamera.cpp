#include "Tutorial/StepGameMoveCamera.h"

#include "Widgets/Tutorial/PanelClickFilter.h"

#include <sgl/graphic/Camera.h>

#include <cmath>

namespace game
{

StepGameMoveCamera::StepGameMoveCamera(int deltaX, int deltaY, float speed)
    : mCamera(sgl::graphic::Camera::GetDefaultCamera())
    , mSpeed(speed)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    Move(deltaX, deltaY);
}

StepGameMoveCamera::~StepGameMoveCamera()
{
    delete mClickFilter;
}

void StepGameMoveCamera::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);
}

void StepGameMoveCamera::Update(float delta)
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

void StepGameMoveCamera::Move(int deltaX, int deltaY)
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
