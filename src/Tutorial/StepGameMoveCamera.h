#pragma once

#include "Tutorial/TutorialStep.h"

namespace sgl
{
    namespace graphic { class Camera; }
}

namespace game
{

class StepGameMoveCamera : public TutorialStep
{
public:
    StepGameMoveCamera(int deltaX, int deltaY, float speed = 500.f);

    void Update(float delta) override;

protected:
    StepGameMoveCamera(float speed);

    void Move(int deltaX, int deltaY);

    sgl::graphic::Camera * GetCamera() const;

private:
    sgl::graphic::Camera * mCamera = nullptr;

    float mSpeed = 0.f;

    int mTargetX = 0;
    int mTargetY = 0;

    float mMovX = 0.f;
    float mMovY = 0.f;

    bool mMovingX = false;
    bool mMovingY = false;
};

inline sgl::graphic::Camera * StepGameMoveCamera::GetCamera() const { return mCamera; }

} // namespace game
