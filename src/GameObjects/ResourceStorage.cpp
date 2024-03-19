#include "GameObjects/ResourceStorage.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

ResourceStorage::ResourceStorage(GameObjectTypeId type, int rows, int cols)
    : Structure(type, CAT_RES_STORAGE, rows, cols)
{
    if(TYPE_RES_STORAGE_BLOBS == type)
        mResource = RES_BLOBS;
    else if(TYPE_RES_STORAGE_DIAMONDS == type)
        mResource = RES_DIAMONDS;
    else if(TYPE_RES_STORAGE_ENERGY == type)
        mResource = RES_ENERGY;
    else if(TYPE_RES_STORAGE_MATERIAL == type)
        mResource = RES_MATERIAL1;
    else
    {
        mResource = RES_INVALID;
        return;
    }

    SetCanBeConquered(true);

    // set capacity based on resource
    const int capacities[NUM_RESOURCES] = { 500, 250, 150, 100 };
    mCapacity = capacities[mResource];

    SetImage();
}

void ResourceStorage::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void ResourceStorage::OnLinkedChanged()
{
    Structure::OnLinkedChanged();

    if(NO_FACTION == GetFaction())
        return ;

    const int diff = IsLinked() ? mCapacity : -mCapacity;

    const Player::Stat statIds[NUM_RESOURCES] =
    {
        Player::Stat::ENERGY,
        Player::Stat::MATERIAL,
        Player::Stat::DIAMONDS,
        Player::Stat::BLOBS
    };

    Player * p = GetScreen()->GetGame()->GetPlayerByFaction(GetFaction());
    p->SumResourceMax(statIds[mResource], diff);
}

void ResourceStorage::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texId = 0;

    if(RES_ENERGY == mResource)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_STORAGE_ENERGY_F1 + (faction * NUM_ENE_STO_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_STORAGE_ENERGY + sel;
    }
    else if(RES_MATERIAL1 == mResource)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_STORAGE_MATERIAL_F1 + (faction * NUM_ENE_STO_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_STORAGE_MATERIAL + sel;
    }
    else if(RES_DIAMONDS == mResource)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_STORAGE_DIAMONDS_F1 + (faction * NUM_ENE_STO_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_STORAGE_DIAMONDS + sel;
    }
    else if(RES_BLOBS == mResource)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_STORAGE_BLOBS_F1 + (faction * NUM_ENE_STO_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_STORAGE_BLOBS + sel;
    }

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texId);
    isoObj->SetTexture(tex);
}

} // namespace game

