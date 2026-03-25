#include "GameObjects/ResourceStorage.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"

#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

ResourceStorage::ResourceStorage(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    if(ObjectData::TYPE_RES_STORAGE_BLOBS == data.GetType())
        mResource = RES_BLOBS;
    else if(ObjectData::TYPE_RES_STORAGE_DIAMONDS == data.GetType())
        mResource = RES_DIAMONDS;
    else if(ObjectData::TYPE_RES_STORAGE_ENERGY == data.GetType())
        mResource = RES_ENERGY;
    else if(ObjectData::TYPE_RES_STORAGE_MATERIAL == data.GetType())
        mResource = RES_MATERIAL1;
    else
    {
        mResource = RES_INVALID;
        return;
    }

    SetCanBeConquered(true);

    UpdateCapacity();

    SetImage();
}

void ResourceStorage::UpdateGraphics()
{
    SetImage();
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

    Player * p = GetOwner();
    p->SumResourceMax(statIds[mResource], diff);
}

void ResourceStorage::OnAttributeChanged()
{
    UpdateCapacity();
}

void ResourceStorage::UpdateCapacity()
{
    const int oldCapacity = mCapacity;

    UpdateCapacityValue();

    // update owner's capacity
    if(oldCapacity != mCapacity && IsLinked())
    {
        const int diff = mCapacity - oldCapacity;

        const Player::Stat statIds[NUM_RESOURCES] =
        {
            Player::Stat::ENERGY,
            Player::Stat::MATERIAL,
            Player::Stat::DIAMONDS,
            Player::Stat::BLOBS
        };

        Player * p = GetOwner();
        p->SumResourceMax(statIds[mResource], diff);
    }
}

void ResourceStorage::UpdateCapacityValue()
{
    Player * p = GetOwner();

    // set max capacity value
    const bool mainRes = RES_ENERGY == mResource || RES_MATERIAL1 == mResource;
    const float maxCapacity = mainRes ? 2000.f : 1000.f;

    // define upgrade multiplier
    float mult = 1.f;

    if(p != nullptr)
    {
        if(RES_ENERGY == mResource)
            mult = p->GetStorageEnergyMult();
        else if(RES_MATERIAL1 == mResource)
            mult = p->GetStorageMaterialMult();
        else if(RES_DIAMONDS == mResource)
            mult = p->GetStorageDiamondsMult();
        else if(RES_BLOBS == mResource)
            mult = p->GetStorageBlobsMult();
    }

    // update capacity value
    mCapacity = std::roundf(mult * maxCapacity * GetAttribute(OBJ_ATT_STORAGE) / MAX_STAT_FVAL);
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

