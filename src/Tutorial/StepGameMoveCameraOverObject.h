#pragma once

#include "Tutorial/StepGameMoveCamera.h"

namespace game
{

class GameObject;

class StepGameMoveCameraOverObject : public StepGameMoveCamera
{
public:
    StepGameMoveCameraOverObject(const GameObject * obj, float speed = 500.f);
};

} // namespace game
