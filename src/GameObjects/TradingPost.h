#pragma once

#include "Structure.h"

namespace game
{

class ObjectData;

class TradingPost : public Structure
{
public:
    TradingPost(const ObjectData & data);

private:
    void UpdateGraphics() override;

    void SetImage();
};

} // namespace game
