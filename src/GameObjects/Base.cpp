#include "GameObjects/Base.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"
#include "Widgets/BlinkingHighlight.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/BinaryFile.h>

#include <cmath>

namespace game
{

Base::Base(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
    , mHighlight(new BlinkingHighlight(SpriteFileStructures, ID_STRUCT_BASE_W))
    , mOutputEnergy(40)
    , mOutputMaterial(20)
{
    SetImage();

    HideHighlight();
}

Base::~Base()
{
    delete mHighlight;
}

bool Base::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = Structure::Load(bf);

    if(!res)
        return false;

    // values
    mOutputEnergy = bf.ReadFloat();
    mOutputMaterial = bf.ReadFloat();

    return true;
}

bool Base::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = Structure::Save(bf);

    if(!res)
        return false;

    // values
    bf.WriteFloat(mOutputEnergy);
    bf.WriteFloat(mOutputMaterial);

    return true;
}

void Base::OnNewTurn(PlayerFaction faction)
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

    const int energy = GetResourceProduction(ER_ENERGY);
    const DataParticleOutput pd1(energy, OT_ENERGY, x1, y12);
    pu->AddParticle(pd1);

    const int material = GetResourceProduction(ER_MATERIAL);
    const DataParticleOutput pd2(material, OT_MATERIAL, x2, y12);
    pu->AddParticle(pd2);

    const int money = GetResourceProduction(ER_MONEY);
    const DataParticleOutput pd3(money, OT_MONEY, x3, y3);
    pu->AddParticle(pd3);
}

int Base::GetResourceProduction(ExtendedResource res) const
{
    auto p = GetOwner();

    if(p == nullptr)
        return 0;

    const float mult = p->GetBaseProductionMult();

    if(res == ER_ENERGY)
        return std::roundf(mult * mOutputEnergy);
    else if(res == ER_MATERIAL)
        return std::roundf(mult * mOutputMaterial);
    else if(res == ER_MONEY)
        return std::roundf(mult * GetGameMap()->GetFactionMoneyPerTurn(GetFaction()));
    else
        return 0;
}


void Base::OnGoalCompleted()
{
    ShowHighlight();

    // play sound
    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/goal_completed.ogg");
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

    unsigned int texInd = ID_STRUCT_BASE_L1;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_BASE_L1;
    else
        texInd = ID_STRUCT_BASE_L1_F1 + faction;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

void Base::HideHighlight()
{
    mHighlight->SetEnabled(false);
    mHighlight->SetVisible(false);
}

void Base::ShowHighlight()
{
    mHighlight->SetEnabled(true);
    mHighlight->SetVisible(true);
}

void Base::OnPositionChanged()
{
    Structure::OnPositionChanged();

    const auto isoObj = GetIsoObject();
    const int isoX = isoObj->GetX();
    const int isoY = isoObj->GetY();

    mHighlight->SetPosition(isoX, isoY);
}

} // namespace game
