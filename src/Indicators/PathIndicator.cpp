#include "Indicators/PathIndicator.h"

#include "GameConstants.h"
#include "GameData.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

PathIndicator::PathIndicator(PlayerFaction faction, bool final)
    : IsoObject(1,1)
    , mFaction(faction)
    , mFinal(final)
{
    UpdateTexture();
}

void PathIndicator::SetFinal(bool final)
{
    // nothing changed
    if(final == mFinal)
        return ;

    mFinal = final;

    UpdateTexture();
}

void PathIndicator::SetDoable(bool doable)
{
    if(doable == mDoable)
        return ;

    mDoable = doable;

    UpdateColors();
}

void PathIndicator::UpdateTexture()
{
    using namespace sgl;

    // update image of indicator and text color of cost
    auto tm = graphic::TextureManager::Instance();

    const unsigned int regularTexIDs[] =
    {
        ID_MIND_PATH_F1,
        ID_MIND_PATH_F2,
        ID_MIND_PATH_F3,
    };

    const unsigned int finalTexIDs[] =
    {
        ID_MIND_PATH_DEST_F1,
        ID_MIND_PATH_DEST_F2,
        ID_MIND_PATH_DEST_F3,
    };

    const unsigned int texID = mFinal ? finalTexIDs[mFaction] : regularTexIDs[mFaction];

    graphic::Texture * tex = tm->GetSprite(SpriteFileMapIndicators, texID);
    SetTexture(tex);
}

void PathIndicator::UpdateColors()
{
    const auto ind = static_cast<unsigned int>(mDoable);

    // set alpha of indicator
    const unsigned char alpha[] = { 64, 255 };
    SetAlpha(alpha[ind]);
}

} // namespace game
