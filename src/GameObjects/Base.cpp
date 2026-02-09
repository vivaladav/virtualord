#include "GameObjects/Base.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

Base::Base(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
    , mOutputEnergy(15)
    , mOutputMaterial(5)
{
    SetImage();
}

void Base::OnNewTurn(PlayerFaction faction)
{
    GameObject::OnNewTurn(faction);

    // not linked yet -> exit
    if(!IsLinked())
        return ;

    // not own turn -> exit
    if(faction != GetFaction())
        return ;

    // AI -> exit
    if(!IsFactionLocal())
        return ;

    // emit notification
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

    IsoObject * isoObj = GetIsoObject();

    const float margin3 = 30.f;
    const float x1 = isoObj->GetX() + isoObj->GetWidth() * 0.25f;
    const float x2 = isoObj->GetX() + isoObj->GetWidth() * 0.75f;
    const float x3 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y12 = isoObj->GetY();
    const float y3 = isoObj->GetY() - margin3;

    const float speed = 40.f;
    const float decaySpeed = 125.f;

    const DataParticleOutput pd1(mOutputEnergy, OT_ENERGY, x1, y12, speed, decaySpeed);
    pu->AddParticle(pd1);

    const DataParticleOutput pd2(mOutputMaterial, OT_MATERIAL, x2, y12, speed, decaySpeed);
    pu->AddParticle(pd2);

    const int money = GetResourceProduction(ER_MONEY);
    const DataParticleOutput pd3(money, OT_MONEY, x3, y3, speed, decaySpeed);
    pu->AddParticle(pd3);
}

int Base::GetResourceProduction(ExtendedResource res) const
{
    if(res == ER_ENERGY)
        return mOutputEnergy;
    else if(res == ER_MATERIAL)
        return mOutputMaterial;
    else if(res == ER_MONEY)
        return GetGameMap()->GetFactionMoneyPerTurn(GetFaction());
    else
        return 0;
}

void Base::UpdateGraphics()
{
    SetImage();
}

void Base::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_BASE_L1;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_BASE_L1 + sel;
    else
        texInd = ID_STRUCT_BASE_L1_F1 + (faction * NUM_BASE_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
