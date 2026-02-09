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
}

void ResearchCenter::OnNewTurn(PlayerFaction faction)
{
    Structure::OnNewTurn(faction);

    // not linked yet -> exit
    if(!IsLinked())
        return ;

    // not own turn -> exit
    if(faction != GetFaction())
        return ;

    // assign research points
    GetOwner()->SumResource(Player::RESEARCH, mResearchPerTurn);

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
    if(res < NUM_EXTENDED_RESOURCES)
        return mResUsage[res];
    else
        return 0;
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

} // namespace game
