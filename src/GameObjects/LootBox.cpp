#include "GameObjects/LootBox.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/BinaryFile.h>
#include <sgl/utilities/LoadedDie.h>
#include <sgl/utilities/UniformDistribution.h>

#include <cassert>
#include <cmath>

namespace game
{

LootBox::LootBox(const ObjectData & data, const ObjectInitData & initData)
    : GameObject(data, initData)
{
    SetImage();

    SetObjColors();

    if(GetObjectType() == ObjectData::TYPE_LOOTBOX2)
    {
        sgl::utilities::LoadedDie die({ 10.f, 90.f });

        // no prize if die returns 0
        if(die.GetNextValue() > 0)
            SetPrize();
    }
    // ObjectData::TYPE_LOOTBOX
    else
       SetPrize();
}

bool LootBox::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = GameObject::Load(bf);

    if(!res)
        return false;

    // values
    mPrizeQuantity = bf.ReadInt();
    mPrizeType = static_cast<Prize>(bf.ReadUint());

    return true;
}

bool LootBox::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = GameObject::Save(bf);

    if(!res)
        return false;

    // values
    bf.WriteInt(mPrizeQuantity);
    bf.WriteUint(mPrizeType);

    return true;
}

bool LootBox::IsExploding() const
{
    return GetObjectType() == ObjectData::TYPE_LOOTBOX2 && mPrizeType == LB_NULL;
}

void LootBox::Open(Player * p)
{
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

    // Lootbox has to explode
    if(IsExploding())
    {
        SelfDestroy();

        if(IsVisible())
            ap->PlaySound("game/explosion-01.ogg");

        return ;
    }
    // give something to player
    else
    {
        Player::Stat stats[] =
        {
            Player::BLOBS,
            Player::DIAMONDS,
            Player::ENERGY,
            Player::MATERIAL,
            Player::MONEY,
            Player::RESEARCH,
        };

        p->SumResource(stats[mPrizeType], mPrizeQuantity);
    }

    // do not show anyting for AI players
    if(p ->IsAI())
        return ;

    // emit notification
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

    const float x0 = GetX() + GetWidth() * 0.5f;
    const float y0 = GetY() - GetHeight() * 0.25f;

    OutputType ot[NUM_OUTPUT_TYPES] =
    {
        OT_BLOBS,
        OT_DIAMONDS,
        OT_ENERGY,
        OT_MATERIAL,
        OT_MONEY,
        OT_RESEARCH,
    };

    static_assert(static_cast<unsigned int>(NUM_OUTPUT_TYPES) ==
                  static_cast<unsigned int>(NUM_LB_PRIZES));

    const float speed = 40.f;
    const float decaySpeed = 100.f;
    const float timeLife = 1.f;
    DataParticleOutput pd(mPrizeQuantity, ot[mPrizeType], x0, y0, speed, decaySpeed, timeLife);

    pu->AddParticle(pd);

    // play SFX
    if(IsVisible())
        ap->PlaySound("game/collect-01.ogg");
}

void LootBox::UpdateGraphics()
{
    SetImage();

    SetObjColors();
}

void LootBox::SetPrize()
{
    // define TYPE of prize
    const std::vector<float> probs = { 15.f, 15.f, 20.f, 20.f, 20.f, 10.f };
    assert(probs.size() == NUM_LB_PRIZES);

    sgl::utilities::LoadedDie die(probs);
    mPrizeType = static_cast<LootBox::Prize>(die.GetNextValue());

    // define QUANTITY of prize
    const int mult = GetObjectType() == ObjectData::TYPE_LOOTBOX2 ? 5 : 1;
    int min;
    int max;

    if(LB_MONEY == mPrizeType)
    {
        min = mult * 100;
        max = mult * 500;
    }
    else if(LB_BLOBS == mPrizeType || LB_DIAMONDS == mPrizeType)
    {
        min = mult * 5;
        max = mult * 25;
    }
    else if(LB_RESEARCH == mPrizeType)
    {
        min = mult * 100;
        max = mult * 300;
    }
    // energy and material
    else
    {
        min = mult * 50;
        max = mult * 250;
    }

    sgl::utilities::UniformDistribution d(min, max);

    // round quantity to 5
    const int r = 5;
    mPrizeQuantity = std::roundf(d.GetNextValue() / static_cast<float>(r)) * r;
}

void LootBox::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int texInd = GetObjectType() == ObjectData::TYPE_LOOTBOX2 ?
                                IND_LOOTBOX_L2 : IND_LOOTBOX;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteCollectiblesFile, texInd);
    isoObj->SetTexture(tex);
}

void LootBox::SetObjColors()
{
    mObjColors.clear();

    if(GetObjectType() == ObjectData::TYPE_LOOTBOX2)
    {
        mObjColors.push_back(0x81a2e4ff);
        mObjColors.push_back(0x2358c3ff);
        mObjColors.push_back(0x5783dbff);
        mObjColors.push_back(0x1b4598ff);
        mObjColors.push_back(0x2d64d2ff);
        mObjColors.push_back(0x13316dff);
    }
    else
    {
        mObjColors.push_back(0xdba457ff);
        mObjColors.push_back(0xd28d2dff);
        mObjColors.push_back(0xe0b06cff);
        mObjColors.push_back(0x805519ff);
        mObjColors.push_back(0x6b4715ff);
        mObjColors.push_back(0x95631dff);
    }
}

} // namespace game
