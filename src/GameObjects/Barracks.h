#pragma once

#include "Structure.h"

namespace game
{

class Barracks : public Structure
{
public:
    Barracks();

protected:
    void UpdateGraphics() override;

private:
    void SetImage();
};

} // namespace game
