#include "GameObjects/ResourceGenerator.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

ResourceGenerator::ResourceGenerator(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    if(ObjectData::TYPE_RES_GEN_ENERGY == data.GetType() ||
       ObjectData::TYPE_RES_GEN_ENERGY_SOLAR == data.GetType())
        mResource = RES_ENERGY;
    else if(ObjectData::TYPE_RES_GEN_MATERIAL == data.GetType() ||
            ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT == data.GetType())
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

int ResourceGenerator::GetResourceProduction(ExtendedResource res) const
{
    if((res == ER_ENERGY && mResource == RES_ENERGY) ||
       (res == ER_MATERIAL && mResource == RES_MATERIAL1))
        return mOutput;
    else
        return 0;
}

void ResourceGenerator::OnNewTurn(PlayerFaction faction)
{
    Structure::OnNewTurn(faction);

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
    OutputType outputType;

    if(type == ObjectData::TYPE_RES_GEN_ENERGY || type == ObjectData::TYPE_RES_GEN_ENERGY_SOLAR)
        outputType = OT_ENERGY;
    else if(type == ObjectData::TYPE_RES_GEN_MATERIAL || type == ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT)
        outputType = OT_MATERIAL;
    else
        return ;

    // emit notification
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

    IsoObject * isoObj = GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY() - isoObj->GetHeight() * 0.1f;

    DataParticleOutput pd(mOutput, outputType, x0, y0);

    pu->AddParticle(pd);
}

void ResourceGenerator::UpdateGraphics()
{
    SetImage();
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

    if(type == ObjectData::TYPE_RES_GEN_ENERGY)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_GEN_ENERGY_F1 + (faction * NUM_ENE_GEN_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_GEN_ENERGY + sel;
    }
    else if(type == ObjectData::TYPE_RES_GEN_MATERIAL)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_GEN_MATERIAL_F1 + (faction * NUM_MAT_GEN_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_GEN_MATERIAL + sel;
    }
    else if(type == ObjectData::TYPE_RES_GEN_ENERGY_SOLAR)
    {
        if(faction != NO_FACTION && IsVisible())
            texId = ID_STRUCT_SOLAR_PANEL_F1 + (faction * NUM_SOLAR_PANEL_SPRITES_PER_FAC) + sel;
        else
            texId = ID_STRUCT_SOLAR_PANEL + sel;
    }
    else if(type == ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT)
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

    if(type == ObjectData::TYPE_RES_GEN_ENERGY)
        mOutput = 80;
    else if(type == ObjectData::TYPE_RES_GEN_MATERIAL)
        mOutput = 50;
    else if(type == ObjectData::TYPE_RES_GEN_ENERGY_SOLAR)
        mOutput = 40;
    else if(type == ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT)
        mOutput = 25;
    // default
    else
        mOutput = 1;
}

} // namespace game

