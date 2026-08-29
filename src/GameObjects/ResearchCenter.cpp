#include "GameObjects/ResearchCenter.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"
#include "Widgets/BlinkingHighlight.h"
#include "Widgets/BlinkingIcon.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/utilities/BinaryFile.h>

namespace game
{

ResearchCenter::ResearchCenter(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
    , mHighlight(new BlinkingHighlight(SpriteFileStructures, ID_STRUCT_RESEARCH_CENTER_W))
    , mIconResearch(new BlinkingIconResearch)
{
    using namespace sgl;

    SetImage();

    UpdateHighlight();

    // init resource usage
    mResUsage.assign(NUM_EXTENDED_RESOURCES, 0);
    mWantedResUsage.assign(NUM_EXTENDED_RESOURCES, 0);

    // set default usage wanted (will be updated in UpdateProduction)
    const int defUsage = 10;

    mWantedResUsage[ER_ENERGY] = defUsage;
    mWantedResUsage[ER_MATERIAL] = defUsage;
    mWantedResUsage[ER_MONEY] = defUsage;

    UpdateProduction();

    // track changes of resources
    auto p = GetOwner();

    if(p != nullptr)
    {
        mResTrackerId = p->AddOnResourcesChanged([this]
                                                 {
                                                     UpdateProduction();
                                                 });

        mResearchTrackerId = p->AddOnResourceChanged(Player::RESEARCH,
                                                     [this](const StatValue *, int, int)
                                                    {
                                                        UpdateHighlight();
                                                    });
    }
}

ResearchCenter::~ResearchCenter()
{
    delete mIconResearch;

    delete mHighlight;

    auto p = GetOwner();

    if(p != nullptr)
    {
        p->RemoveOnResourcesChanged(mResTrackerId);
        p->RemoveOnResourceChanged(Player::RESEARCH, mResearchTrackerId);
    }
}

bool ResearchCenter::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = Structure::Load(bf);

    if(!res)
        return false;

    // resource usage
    const unsigned int numRes = bf.ReadUint();

    for(unsigned int i = 0; i < numRes; ++i)
        mWantedResUsage[i] = bf.ReadInt();

    // values
    mAlphaAnim = bf.ReadFloat();

    // update production
    UpdateProduction();

    return true;
}

bool ResearchCenter::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = Structure::Save(bf);

    if(!res)
        return false;

    // resource usage
    const unsigned int numRes = mWantedResUsage.size();
    bf.WriteUint(numRes);

    for(int val : mWantedResUsage)
        bf.WriteInt(val);

    // values
    bf.WriteFloat(mAlphaAnim);

    return true;
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

    const int marginV = 20;
    const float x = GetX() + GetWidth() / 2;
    const float y = GetY() - marginV;

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

void ResearchCenter::SetWantedResourceUsage(ExtendedResource res, int val)
{
    if(res >= NUM_EXTENDED_RESOURCES)
        return ;

    if(mWantedResUsage[res] == val)
        return ;

    mWantedResUsage[res] = val;

    UpdateProduction();
}

int ResearchCenter::GetWantedResourceUsage(ExtendedResource res) const
{
    if(res < NUM_EXTENDED_RESOURCES)
        return mWantedResUsage[res];
    else
        return 0;
}

void ResearchCenter::OnPositionChanged()
{
    Structure::OnPositionChanged();

    mHighlight->SetPosition(GetX(), GetY());
}

void ResearchCenter::UpdateGraphics()
{
    Structure::UpdateGraphics();

    SetImage();

    PositionIconResearch();
}

void ResearchCenter::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();

    unsigned int texInd = ID_STRUCT_RESEARCH_CENTER;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RESEARCH_CENTER;
    else
        texInd = ID_STRUCT_RESEARCH_CENTER_F1 + faction;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

void ResearchCenter::UpdateProduction()
{
    // reset usage
    mResUsage.assign(NUM_EXTENDED_RESOURCES, 0);

    // check owner
    auto p = GetOwner();

    if(p == nullptr)
    {
        mResearchPerTurn = 0;
        UpdateIconResearch();
        return ;
    }

    // -- clamp usage to what's available --
    // ENERGY
    const int availEnergy = p->GetStat(Player::ENERGY).GetValue() -
                            p->GetResourceConsumption(ER_ENERGY);

    if(availEnergy < 0)
        mResUsage[ER_ENERGY] = 0;
    else if(mWantedResUsage[ER_ENERGY] > availEnergy)
        mResUsage[ER_ENERGY] = availEnergy;
    else
        mResUsage[ER_ENERGY] = mWantedResUsage[ER_ENERGY];

    // MATERIAL
    const int availMaterial = p->GetStat(Player::MATERIAL).GetValue() -
                              p->GetResourceConsumption(ER_MATERIAL);

    if(availMaterial < 0)
        mResUsage[ER_MATERIAL] = 0;
    else if(mWantedResUsage[ER_MATERIAL] > availMaterial)
        mResUsage[ER_MATERIAL] = availMaterial;
    else
        mResUsage[ER_MATERIAL] = mWantedResUsage[ER_MATERIAL];

    // MONEY
    const int availMoney = p->GetStat(Player::MONEY).GetValue() -
                            p->GetResourceConsumption(ER_MONEY);

    if(availMoney < 0)
        mResUsage[ER_MONEY] = 0;
    else if(mWantedResUsage[ER_MONEY] > availMoney)
        mResUsage[ER_MONEY] = availMoney;
    else
        mResUsage[ER_MONEY] = mWantedResUsage[ER_MONEY];

    // no production if required resource is not used
    if(mResUsage[ER_ENERGY] == 0 || mResUsage[ER_MATERIAL] == 0 || mResUsage[ER_MONEY] == 0)
    {
        mResearchPerTurn = 0;
        UpdateIconResearch();
        return ;
    }

    // BLOBS
    const int availBlobs = p->GetStat(Player::BLOBS).GetValue() -
                              p->GetResourceConsumption(ER_BLOBS);

    if(availBlobs < 0)
        mResUsage[ER_BLOBS] = 0;
    else if(mWantedResUsage[ER_BLOBS] > availBlobs)
        mResUsage[ER_BLOBS] = availBlobs;
    else
        mResUsage[ER_BLOBS] = mWantedResUsage[ER_BLOBS];

    // DIAMONDS
    const int availDiamonds = p->GetStat(Player::DIAMONDS).GetValue() -
                              p->GetResourceConsumption(ER_DIAMONDS);

    if(availDiamonds < 0)
        mResUsage[ER_DIAMONDS] = 0;
    else if(mWantedResUsage[ER_DIAMONDS] > availDiamonds)
        mResUsage[ER_DIAMONDS] = availDiamonds;
    else
        mResUsage[ER_DIAMONDS] = mWantedResUsage[ER_DIAMONDS];

    // -- define research points production --
    const int maxProdElem = 40;
    const int maxProdElem2 = 60;
    const int maxUsage = 100;
    const int baseProd = (maxProdElem * mResUsage[ER_ENERGY] / maxUsage) +
                         (maxProdElem * mResUsage[ER_MATERIAL] / maxUsage) +
                         (maxProdElem2 * mResUsage[ER_MONEY] / maxUsage);
    const int multProd2 = 2;

    mResearchPerTurn = baseProd + (baseProd * multProd2 * mResUsage[ER_BLOBS] / maxUsage) +
                       (baseProd * multProd2 * mResUsage[ER_DIAMONDS] / maxUsage);

    UpdateIconResearch();
}

void ResearchCenter::HideIconResearch()
{
    mIconResearch->SetVisible(false);
    mIconResearch->SetEnabled(false);
}

void ResearchCenter::ShowIconResearch()
{
    if(!IsFactionLocal())
        return ;

    mIconResearch->SetVisible(true);
    mIconResearch->SetEnabled(true);

    PositionIconResearch();
}

void ResearchCenter::PositionIconResearch()
{
    const int x0 = GetX();
    const int y0 = GetStatusIconBaseY();

    const int iconMarginV = 5;
    const int iconX = x0 + (GetWidth() - mIconResearch->GetWidth()) / 2;
    const int iconY = y0 - mIconResearch->GetHeight() - iconMarginV;

    mIconResearch->SetPosition(iconX, iconY);
}

void ResearchCenter::UpdateIconResearch()
{
    if(mResearchPerTurn == 0 && IsLinked() && IsFactionLocal())
        ShowIconResearch();
    else
        HideIconResearch();
}

void ResearchCenter::HideHighlight()
{
    mHighlight->SetEnabled(false);
    mHighlight->SetVisible(false);
}

void ResearchCenter::ShowHighlight()
{
    mHighlight->SetEnabled(true);
    mHighlight->SetVisible(true);
}

void ResearchCenter::UpdateHighlight()
{
    // structure not local or not working -> HIDE
    if(!IsFactionLocal() || !IsLinked())
    {
        HideHighlight();
        return ;
    }

    const int resPoints = GetOwner()->GetStat(Player::RESEARCH).GetValue();

    auto game = GetGame();
    auto player = GetOwner();

    for(unsigned int i = 0; i < NUM_TECH_UPGRADES; ++i)
    {
        const auto tu = static_cast<TechUpgradeId>(i);

        // there's at least 1 available upgrade that can be unlocked -> SHOW
        if(!player->IsUpgradeUnlocked(tu) && player->IsUpgradeAvailable(tu) &&
           game->GetTechUpgradecost(tu) <= resPoints)
        {
            ShowHighlight();
            return ;
        }
    }

    // none found -> HIDE
    HideHighlight();
}

void ResearchCenter::OnFactionChanged()
{
    Structure::OnFactionChanged();

    UpdateHighlight();
}

void ResearchCenter::OnLinkedChanged()
{
    Structure::OnLinkedChanged();

    UpdateHighlight();
}

} // namespace game
