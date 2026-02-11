#include "GameObjects/ResearchCenter.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

ResearchCenter::ResearchCenter(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetImage();

    // init resource usage
    mResUsage.assign(NUM_EXTENDED_RESOURCES, 0);
    mResUsage[ER_ENERGY] = 10;
    mResUsage[ER_MATERIAL] = 10;
    mResUsage[ER_MONEY] = 10;

    UpdateProduction();

    // track changes
    auto p = GetOwner();

    if(p != nullptr)
        mResTrackerId = p->AddOnResourcesChanged([this]
    {
        UpdateProduction();
    });
}

ResearchCenter::~ResearchCenter()
{
    auto p = GetOwner();

    if(p != nullptr)
        p->RemoveOnResourcesChanged(mResTrackerId);
}

void ResearchCenter::OnNewTurn(PlayerFaction faction)
{
    Structure::OnNewTurn(faction);

    // not producing anything
    if(mResearchPerTurn == 0)
        return ;

    // not linked yet -> exit
    if(!IsLinked())
        return ;

    // not own turn -> exit
    if(faction != GetFaction())
        return ;

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

    const DataParticleOutput pd(mResearchPerTurn, OT_RESEARCH, x, y, speed, decaySpeed);
    pu->AddParticle(pd);
}

int ResearchCenter::GetResourceProduction(ExtendedResource res) const
{
    if(res == ER_RESEARCH)
        return mResearchPerTurn;
    else
        return 0;
}

int ResearchCenter::GetResourceUsage(ExtendedResource res) const
{
    // not consuming while not producing (for example when there's not enough resources)
    if(mResearchPerTurn == 0)
        return 0;

    if(res < NUM_EXTENDED_RESOURCES)
        return mResUsage[res];
    else
        return 0;
}

void ResearchCenter::SetResourceUsage(ExtendedResource res, int val)
{
    if(res >= NUM_EXTENDED_RESOURCES)
        return ;

    if(mResUsage[res] == val)
        return ;

    mResUsage[res] = val;

    UpdateProduction();
}

void ResearchCenter::UpdateGraphics()
{
    SetImage();
}

void ResearchCenter::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_RESEARCH_CENTER;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RESEARCH_CENTER + sel;
    else
        texInd = ID_STRUCT_RESEARCH_CENTER_F1 + (faction * NUM_RESEARCH_CENTER_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

void ResearchCenter::UpdateProduction()
{
    // check owner
    auto p = GetOwner();

    if(p == nullptr)
    {
        mResearchPerTurn = 0;
        return ;
    }

    // ENERGY usage
    const int energy = p->GetStat(Player::ENERGY).GetValue();
    const int usageEnergy = mResUsage[ER_ENERGY] > energy ? energy : mResUsage[ER_ENERGY];

    // MATERIAL usage
    const int material = p->GetStat(Player::MATERIAL).GetValue();
    const int usageMaterial = mResUsage[ER_MATERIAL] > material ? material : mResUsage[ER_MATERIAL];

    // MONEY usage
    const int money = p->GetStat(Player::MONEY).GetValue();
    const int usageMoney = mResUsage[ER_MONEY] > money ? money : mResUsage[ER_MONEY];

    // no production if required resource is not used
    if(usageEnergy == 0 || usageMaterial == 0 || usageMoney == 0)
    {
        mResearchPerTurn = 0;
        return ;
    }

    // BLOBS uasge
    const int blobs = p->GetStat(Player::BLOBS).GetValue();
    const int usageBlobs = mResUsage[ER_BLOBS] > blobs ? blobs : mResUsage[ER_BLOBS];

    // DIAMONDS uasge
    const int diamonds = p->GetStat(Player::DIAMONDS).GetValue();
    const int usageDiamonds = mResUsage[ER_DIAMONDS] > diamonds ? diamonds : mResUsage[ER_DIAMONDS];

    const int maxProdElem = 30;
    const int maxProdElem2 = 40;
    const int maxUsage = 100;
    const int baseProd = (maxProdElem * usageEnergy / maxUsage) +
                         (maxProdElem * usageMaterial / maxUsage) +
                         (maxProdElem2 * usageMoney / maxUsage);
    const int multProd2 = 2;

    mResearchPerTurn = baseProd + (baseProd * multProd2 * usageBlobs / maxUsage) +
                       (baseProd * multProd2 * usageDiamonds / maxUsage);
}

} // namespace game
