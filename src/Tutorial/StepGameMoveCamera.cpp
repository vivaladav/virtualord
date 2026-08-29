#include "Tutorial/StepGameMoveCamera.h"

#include <sgl/graphic/Camera.h>

#include <cmath>

namespace game
{

StepGameMoveCamera::StepGameMoveCamera(int deltaX, int deltaY, float speed)
    : StepGameMoveCamera(speed)
{
    Move(deltaX, deltaY);
}

StepGameMoveCamera::StepGameMoveCamera(float speed)
    : mCamera(sgl::graphic::Camera::GetDefaultCamera())
    , mSpeed(speed)
{
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
    // make sure both delta are not 0 or there's nothing to do
    if(deltaX == 0 && deltaY == 0)
    {
        SetDone();
        return;
    }

    mTargetX = mCamera->GetX() + deltaX;
    mTargetY = mCamera->GetY() + deltaY;

    // set speed multipliers to make sure X and Y movement finish at the same time
    float multX = 1.f;
    float multY = 1.f;

    // if any of the 2 deltas is 0 there's no point in calculating the mult
    if(deltaX != 0 && deltaY != 0)
    {
        if(deltaX > deltaY)
            multX = std::fabs(static_cast<float>(deltaX) / static_cast<float>(deltaY));
        else
            multY = std::fabs(static_cast<float>(deltaY) / static_cast<float>(deltaX));
    }

    // normalize multiplier values to avoid spikes of speed
    if(multX > multY)
    {
        if(multX > 1.f)
        {
            multY /= multX;
            multX = 1.f;
        }
    }
    else if(multY > multX)
    {
        if(multY > 1.f)
        {
            multX /= multY;
            multY = 1.f;
        }
    }

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
        mMovY = mSpeed * multY;
    else if(deltaY < 0)
        mMovY = -mSpeed * multY;
    else
    {
        mMovY = 0.f;
        mMovingY = false;
    }
}

} // namespace game
