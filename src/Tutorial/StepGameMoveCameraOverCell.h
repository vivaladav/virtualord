#pragma once

#include "Cell2D.h"
#include "Tutorial/StepGameMoveCamera.h"

namespace game
{

class IsoMap;

class StepGameMoveCameraOverCell : public StepGameMoveCamera
{
public:
    StepGameMoveCameraOverCell(const Cell2D & cell, const IsoMap * im, float speed = 500.f);
};

} // namespace game
