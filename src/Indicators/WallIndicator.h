#pragma once

#include "IsoObject.h"
#include "GameObjects/GameObjectTypes.h"

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }
}

namespace game
{

enum PlayerFaction : unsigned int;

class WallIndicator : public IsoObject
{
public:
    WallIndicator(PlayerFaction faction);

    GameObjectVariantId GetBlockType() const;

    void SetBeforeAfterDirections(int br, int bc, int ar, int ac);

private:
    void CreateCostData();

    void UpdateImage();

private:
    enum WallBlock : unsigned int
    {
        WB_HORIZONTAL,
        WB_VERTICAL,
        WB_TOP_LEFT,
        WB_TOP_RIGHT,
        WB_BOTTOM_LEFT,
        WB_BOTTOM_RIGHT,

        NUM_WALL_BLOCKS,

        WB_INVALID
    };

    WallBlock mBlock = WB_INVALID;

    PlayerFaction mFaction;
};

} // namespace game
