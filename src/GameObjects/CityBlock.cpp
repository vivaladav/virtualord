#include "GameObjects/CityBlock.h"

#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/CityGroup.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

CityBlock::CityBlock(const ObjectData & data, const ObjectInitData & initData,
                         GameObjectVariantId part)
    : Structure(data, initData)
{
    SetCanBeConquered(true);

    mVariant = part;

    // set object health
    const float maxHealthValue = 2000.f;
    SetMaxHealth(maxHealthValue);

    SetImage();
}

void CityBlock::OnNewTurn(PlayerFaction faction)
{
    Structure::OnNewTurn(faction);

    // not linked yet -> exit
    if(!IsLinked())
        return ;

    // not own turn -> exit
    if(faction != GetFaction())
        return ;

    auto g = static_cast<CityGroup *>(GetGroup());

    // city not conquered yet -> exit
    if(g == nullptr || !g->IsCityConquered())
        return ;

    // assign money
    const int money = GetResourceProduction(ER_MONEY);

    // VISUAL NOTIFICATION
    // AI -> exit
    if(!IsFactionLocal())
        return ;

    // emit notification
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

    IsoObject * isoObj = GetIsoObject();

    const int marginV = 20;
    const float x = isoObj->GetX() + isoObj->GetWidth() / 2;
    const float y = isoObj->GetY() - marginV;

    const float speed = 45.f;
    const float decaySpeed = 120.f;

    const DataParticleOutput pd(money, OT_MONEY, x, y, speed, decaySpeed);
    pu->AddParticle(pd);
}

int CityBlock::GetResourceProduction(ExtendedResource res) const
{
    if(res == ER_MONEY)
    {
        const int money = 50;
        return money;
    }
    else
        return 0;
}

void CityBlock::UpdateGraphics()
{
    SetImage();
}

void CityBlock::OnFactionChanged()
{
    Structure::OnFactionChanged();

    auto g = static_cast<CityGroup *>(GetGroup());

    if(g != nullptr)
        g->UpdateCityConquered(GetOwner());
}

void CityBlock::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    // set texture
    auto tm = sgl::graphic::TextureManager::Instance();

    sgl::graphic::Texture * tex = nullptr;

    const int sel = static_cast<int>(IsSelected());
    const PlayerFaction f = GetFaction();

    const unsigned int factionSpriteIds[] =
    {
        ID_CITY_TL1_F1,
        ID_CITY_TL1_F2,
        ID_CITY_TL1_F3,
    };

    static_assert((sizeof(factionSpriteIds) / sizeof(unsigned int)) == NUM_FACTIONS);

    const unsigned int spriteId0 = f != NO_FACTION ? factionSpriteIds[f] : ID_CITY_TL1_NF;
    const unsigned int spriteId = spriteId0 + mVariant + (sel * NUM_CITY_SPRITES);
    tex = tm->GetSprite(SpriteFileCity, spriteId);

    isoObj->SetTexture(tex);
}

} // namespace game
