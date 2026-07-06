#include "Tutorial/StepGameMoveCameraOverObject.h"

#include "IsoObject.h"
#include "GameObjects/GameObject.h"

#include <sgl/graphic/Camera.h>

#include <cmath>

namespace game
{

StepGameMoveCameraOverObject::StepGameMoveCameraOverObject(const GameObject * obj, float speed)
    : StepGameMoveCamera(speed)
{
    // DEFINE MOVE
    const IsoObject * isoObj = obj->GetIsoObject();
    const int cX = isoObj->GetX() + isoObj->GetWidth() / 2;
    const int cY = isoObj->GetY() + isoObj->GetHeight() / 2;

    const auto camera = GetCamera();
    const int deltaX = cX - (camera->GetWidth() / 2) - camera->GetX();
    const int deltaY = cY - (camera->GetHeight() / 2) - camera->GetY();

    Move(deltaX, deltaY);
}


} // namespace game
