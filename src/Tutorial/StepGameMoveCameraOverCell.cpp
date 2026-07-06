#include "Tutorial/StepGameMoveCameraOverCell.h"

#include "IsoMap.h"

#include <sgl/graphic/Camera.h>

#include <cmath>

namespace game
{

StepGameMoveCameraOverCell::StepGameMoveCameraOverCell(const Cell2D & cell, const IsoMap * im,
                                                       float speed)
    : StepGameMoveCamera(speed)
{
    // DEFINE MOVE
    const sgl::core::Pointd2D p = im->GetCellPosition(cell.row, cell.col);
    const int cX = p.x + im->GetTileWidth() / 2;
    const int cY = p.y + im->GetTileHeight() / 2;

    const auto camera = GetCamera();
    const int deltaX = cX - (camera->GetWidth() / 2) - camera->GetX();
    const int deltaY = cY - (camera->GetHeight() / 2) - camera->GetY();

    Move(deltaX, deltaY);
}


} // namespace game
