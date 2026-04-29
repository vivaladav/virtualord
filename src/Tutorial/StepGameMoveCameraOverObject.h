#pragma once

#include "Tutorial/TutorialStep.h"

namespace sgl
{
    namespace graphic { class Camera; }
}

namespace game
{

class GameObject;
class PanelClickFilter;

class StepGameMoveCameraOverObject : public TutorialStep
{
public:
    StepGameMoveCameraOverObject(const GameObject * obj, float speed = 400.f);
    ~StepGameMoveCameraOverObject();

    void OnStart() override;

    void Update(float delta) override;

private:
    void Move(int deltaX, int deltaY);

private:
    PanelClickFilter * mClickFilter = nullptr;

    sgl::graphic::Camera * mCamera = nullptr;

    float mSpeed = 0.f;

    int mTargetX = 0;
    int mTargetY = 0;

    float mMovX = 0.f;
    float mMovY = 0.f;

    bool mMovingX = false;
    bool mMovingY = false;
};

} // namespace game
