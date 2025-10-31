#include "GameObjects/Base.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "GameObjects/LootBox.h"
#include "Particles/DataParticleLootboxPrize.h"
#include "Particles/UpdaterLootboxPrize.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

Base::Base()
    : Structure(GameObject::TYPE_BASE, GameObject::CAT_GENERIC, 3, 3)
    , mOutputEnergy(15)
    , mOutputMaterial(5)
{
    SetVisibilityLevel(6);
    // base is linked to itself
    SetLinked(true);
}

void Base::OnNewTurn(PlayerFaction faction)
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

    // emit notification
    auto pu = static_cast<UpdaterLootboxPrize *>(GetScreen()->GetParticleUpdater(PU_LOOTBOX_PRIZE));

    IsoObject * isoObj = GetIsoObject();

    const float margin3 = 30.f;
    const float x1 = isoObj->GetX() + isoObj->GetWidth() * 0.25f;
    const float x2 = isoObj->GetX() + isoObj->GetWidth() * 0.75f;
    const float x3 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y12 = isoObj->GetY();
    const float y3 = isoObj->GetY() - margin3;

    const float speed = 40.f;
    const float decaySpeed = 125.f;

    const DataParticleLootboxPrize pd1(mOutputEnergy, LootBox::LB_ENERGY, x1, y12, speed, decaySpeed);
    pu->AddParticle(pd1);

    const DataParticleLootboxPrize pd2(mOutputMaterial, LootBox::LB_MATERIAL, x2, y12, speed, decaySpeed);
    pu->AddParticle(pd2);

    const int money = GetGameMap()->GetFactionMoneyPerTurn(faction);
    const DataParticleLootboxPrize pd3(money, LootBox::LB_MONEY, x3, y3, speed, decaySpeed);
    pu->AddParticle(pd3);
}

void Base::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
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
