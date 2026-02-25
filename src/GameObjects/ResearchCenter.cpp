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

    // set default usage wanted (will be updated in UpdateProduction)
    const int defUsage = 10;

    mResUsage[ER_ENERGY] = defUsage;
    mResUsage[ER_MATERIAL] = defUsage;
    mResUsage[ER_MONEY] = defUsage;

    UpdateProduction();

    // track changes of resources
    auto p = GetOwner();

    if(p != nullptr)
    {
        mResTrackerId = p->AddOnResourcesChanged([this]
                                                 {
                                                     UpdateProduction();
                                                 });
    }
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

    const DataParticleOutput pd(mResearchPerTurn, OT_RESEARCH, x, y);
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

    // -- clamp usage to what's available --
    // ENERGY
    const int energy = p->GetStat(Player::ENERGY).GetValue();

    if(mResUsage[ER_ENERGY] > energy)
        mResUsage[ER_ENERGY] = energy;

    // MATERIAL
    const int material = p->GetStat(Player::MATERIAL).GetValue();

    if(mResUsage[ER_MATERIAL] > material)
        mResUsage[ER_MATERIAL] = material;

    // MONEY
    const int money = p->GetStat(Player::MONEY).GetValue();

    if(mResUsage[ER_MONEY] > money)
        mResUsage[ER_MONEY] = money;

    // no production if required resource is not used
    if(mResUsage[ER_ENERGY] == 0 || mResUsage[ER_MATERIAL] == 0 || mResUsage[ER_MONEY] == 0)
    {
        mResearchPerTurn = 0;
        return ;
    }

    // BLOBS
    const int blobs = p->GetStat(Player::BLOBS).GetValue();

    if(mResUsage[ER_BLOBS] > blobs)
        mResUsage[ER_BLOBS] = blobs;

    // DIAMONDS
    const int diamonds = p->GetStat(Player::DIAMONDS).GetValue();

    if(mResUsage[ER_DIAMONDS] > diamonds)
        mResUsage[ER_DIAMONDS] = diamonds;

    // -- define research points production --
    const int maxProdElem = 30;
    const int maxProdElem2 = 40;
    const int maxUsage = 100;
    const int baseProd = (maxProdElem * mResUsage[ER_ENERGY] / maxUsage) +
                         (maxProdElem * mResUsage[ER_MATERIAL] / maxUsage) +
                         (maxProdElem2 * mResUsage[ER_MONEY] / maxUsage);
    const int multProd2 = 2;

    mResearchPerTurn = baseProd + (baseProd * multProd2 * mResUsage[ER_BLOBS] / maxUsage) +
                       (baseProd * multProd2 * mResUsage[ER_DIAMONDS] / maxUsage);
}

} // namespace game
