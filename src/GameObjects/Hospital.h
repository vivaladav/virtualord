#pragma once

#include "Structure.h"

namespace game
{

class Hospital : public Structure
{
public:
    Hospital();

protected:
    void UpdateGraphics() override;

private:
    void SetImage();
};

} // namespace game
