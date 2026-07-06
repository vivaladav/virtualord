#include "Indicators/WallIndicator.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameObjects/Wall.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

WallIndicator::WallIndicator(PlayerFaction faction)
    : IsoObject(1, 1)
    , mFaction(faction)
{
    UpdateImage();
}

void WallIndicator::SetBeforeAfterDirections(int br, int bc, int ar, int ac)
{
    // -1 direction away from the center
    // +1 direction toward the center
    // 0 no direction

    // special case: all directions are 0 (it's the only block of a wall)
    if(0 == br && 0 == bc && 0 == ar && 0 == ac)
        mBlock = WB_HORIZONTAL;
    else if(1 == bc)
    {
        if(1 == ar)
            mBlock = WB_TOP_RIGHT;
        else if(-1 == ar)
            mBlock = WB_BOTTOM_RIGHT;
        else
            mBlock = WB_HORIZONTAL;
    }
    else if(-1 == bc)
    {
        if(1 == ar)
            mBlock = WB_TOP_LEFT;
        else if(-1 == ar)
            mBlock = WB_BOTTOM_LEFT;
        else
            mBlock = WB_HORIZONTAL;
    }
    else if(1 == br)
    {
        if(1 == ac)
            mBlock = WB_BOTTOM_LEFT;
        else if(-1 == ac)
            mBlock = WB_BOTTOM_RIGHT;
        else
            mBlock = WB_VERTICAL;
    }
    else if(-1 == br)
    {
        if(1 == ac)
            mBlock = WB_TOP_LEFT;
        else if(-1 == ac)
            mBlock = WB_TOP_RIGHT;
        else
            mBlock = WB_VERTICAL;
    }
    // bc and br are 0
    else
    {
        if(1 == ar || -1 == ar)
            mBlock = WB_VERTICAL;
        else if(1 == ac || -1 == ac)
            mBlock = WB_HORIZONTAL;
        else
            mBlock = WB_INVALID;
    }

    UpdateImage();
}

void WallIndicator::SetDoable(bool doable)
{
    if(mDoable == doable)
        return ;

    mDoable = doable;

    const unsigned int alpha[] = { 150, 255};

    SetAlpha(alpha[doable]);
}

GameObjectVariantId WallIndicator::GetBlockType() const
{
    static const GameObjectVariantId types[] =
    {
        Wall::WallPart::HORIZ,
        Wall::WallPart::VERT,
        Wall::WallPart::TL,
        Wall::WallPart::TR,
        Wall::WallPart::BL,
        Wall::WallPart::BR,
    };

    if(WB_INVALID == mBlock)
        return Wall::WallPart::HORIZ;
    else
        return types[mBlock];
}

void WallIndicator::UpdateImage()
{
    using namespace sgl::graphic;

    auto tm = TextureManager::Instance();

    // not in a valid state
    if(NO_FACTION == mFaction || WB_INVALID == mBlock)
    {
        SetTexture(tm->GetSprite(SpriteFileMapIndicators, IND_WB_INVALID));
        return ;
    }

    // upate object body
    const unsigned int index = IND_WB_FIRST + (mFaction * NUM_WALL_BLOCKS) + mBlock;
    SetTexture(tm->GetSprite(SpriteFileMapIndicators, index));
}


} // namespace game
