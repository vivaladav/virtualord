#include "GameObjects/ResourceGenerator.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "GameObjects/LootBox.h"
#include "Particles/DataParticleLootboxPrize.h"
#include "Particles/UpdaterLootboxPrize.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

ResourceGenerator::ResourceGenerator(GameObjectTypeId type, int rows, int cols)
    : Structure(type, CAT_RES_GENERATOR, rows, cols)
{
    if(TYPE_RES_GEN_ENERGY == type || TYPE_RES_GEN_ENERGY_SOLAR == type)
        mResource = RES_ENERGY;
    else if(TYPE_RES_GEN_MATERIAL == type || TYPE_RES_GEN_MATERIAL_EXTRACT == type)
        mResource = RES_MATERIAL1;
    else
    {
        mResource = RES_INVALID;
        return ;
    }

    SetCanBeConquered(true);

    SetImage();

    UpdateOutput();
}

void ResourceGenerator::ScaleOutput(float mult)
{
    const float minOutput = 1.f;
    const float output = mOutput * mult;

    if(output < minOutput)
        mOutput = minOutput;
    else
        mOutput = std::roundf(output);
}

void ResourceGenerator::OnNewTurn(PlayerFaction faction)
{
    // not linked yet -> exit
    if(!IsLinked())
        return ;

    // not own turn -> exit
    if(faction != GetFaction())
        return ;

    // AI -> exit
    if(!IsFactionLocal())
        return ;

    const GameObjectTypeId type = GetObjectType();
    unsigned int outputType;

    if(type == GameObject::TYPE_RES_GEN_ENERGY || type == GameObject::TYPE_RES_GEN_ENERGY_SOLAR)
        outputType = LootBox::LB_ENERGY;
    else if(type == GameObject::TYPE_RES_GEN_MATERIAL || type == GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT)
        outputType = LootBox::LB_MATERIAL;
    else
        return ;

    // emit notification
    auto pu = static_cast<UpdaterLootboxPrize *>(GetScreen()->GetParticleUpdater(PU_LOOTBOX_PRIZE));

    IsoObject * isoObj = GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY() - isoObj->GetHeight() * 0.1f;

    const float speed = 40.f;
    const float decaySpeed = 125.f;

    DataParticleLootboxPrize pd(mOutput, outputType, x0, y0, speed, decaySpeed);

    pu->AddParticle(pd);
}

void ResourceGenerator::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void ResourceGenerator::SetImage()
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

    const GameObjectTypeId type = GetObjectType();

    if(type == TYPE_RES_GEN_ENERGY)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_GEN_ENERGY_F1 + (faction * NUM_ENE_GEN_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_GEN_ENERGY + sel;
    }
    else if(type == TYPE_RES_GEN_MATERIAL)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_GEN_MATERIAL_F1 + (faction * NUM_MAT_GEN_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_GEN_MATERIAL + sel;
    }
    else if(type == TYPE_RES_GEN_ENERGY_SOLAR)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_SOLAR_PANEL_F1 + (faction * NUM_SOLAR_PANEL_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_SOLAR_PANEL + sel;
    }
    else if(type == TYPE_RES_GEN_MATERIAL_EXTRACT)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_MATERIAL_EXTRACTOR_F1 + (faction * NUM_MATERIAL_EXTRACTOR_SPRITES_PER_FAC) + sel;
        else
            texId = ID_MATERIAL_EXTRACTOR + sel;
    }

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texId);
    isoObj->SetTexture(tex);
}

void ResourceGenerator::UpdateOutput()
{
    const GameObjectTypeId type = GetObjectType();

    if(type == TYPE_RES_GEN_ENERGY)
        mOutput = 50;
    else if(type == TYPE_RES_GEN_MATERIAL)
        mOutput = 25;
    else if(type == TYPE_RES_GEN_ENERGY_SOLAR)
        mOutput = 15;
    else if(type == TYPE_RES_GEN_MATERIAL_EXTRACT)
        mOutput = 10;
    // default
    else
        mOutput = 1;
}

} // namespace game

