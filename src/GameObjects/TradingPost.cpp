#include "GameObjects/TradingPost.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "ObjectData.h"

#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

TradingPost::TradingPost(const ObjectData & data)
    : Structure(GameObject::TYPE_TRADING_POST, GameObject::CAT_GENERIC, data.GetRows(), data.GetCols())
{
    SetVisibilityLevel(4);

    SetImage();
}

void TradingPost::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void TradingPost::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_TRADING_POST;

    if(NO_FACTION == faction)
        texInd += sel;
    else
        texInd = ID_STRUCT_TRADING_POST_F1 + (faction * NUM_TRADIN_POST_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
