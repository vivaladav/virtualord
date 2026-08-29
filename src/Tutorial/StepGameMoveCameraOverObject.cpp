#include "Tutorial/StepGameMoveCameraOverObject.h"

#include "GameObjects/GameObject.h"

#include <sgl/graphic/Camera.h>

#include <cmath>

namespace game
{

StepGameMoveCameraOverObject::StepGameMoveCameraOverObject(const GameObject * obj, float speed)
    : StepGameMoveCamera(speed)
{
    // DEFINE MOVE
    const int cX = obj->GetX() + obj->GetWidth() / 2;
    const int cY = obj->GetY() + obj->GetHeight() / 2;

    const auto camera = GetCamera();
    const int deltaX = cX - (camera->GetWidth() / 2) - camera->GetX();
    const int deltaY = cY - (camera->GetHeight() / 2) - camera->GetY();

    Move(deltaX, deltaY);
}


} // namespace game
