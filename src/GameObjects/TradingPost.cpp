#include "GameObjects/TradingPost.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "ObjectData.h"

#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

TradingPost::TradingPost(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetImage();
}

void TradingPost::UpdateGraphics()
{
    SetImage();
}

void TradingPost::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();

    unsigned int texInd = ID_STRUCT_TRADING_POST;

    if(faction != NO_FACTION)
        texInd = ID_STRUCT_TRADING_POST_F1 + faction;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
