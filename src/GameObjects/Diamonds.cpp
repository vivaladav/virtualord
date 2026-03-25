#include "GameObjects/Diamonds.h"

#include "Game.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/UniformDistribution.h>

namespace game
{

const int MIN_UNITS = 1;
const int MAX_UNITS = 4;

Diamonds::Diamonds(const ObjectData & data, const ObjectInitData & initData)
    : Collectable(data, initData)
{
    sgl::utilities::UniformDistribution ran(MIN_UNITS, MAX_UNITS, GetGame()->GetRandSeed());
    mNum = ran.GetNextValue();

    SetObjColors();
    SetImage();
}

void Diamonds::MaximizeUnits()
{
    mNum = MAX_UNITS;

    UpdateGraphics();
}

void Diamonds::MinimizeUnits()
{
    mNum = MIN_UNITS;

    UpdateGraphics();
}

void Diamonds::Collected(Player * collector)
{
    Collectable::Collected(collector);

    // do not show anyting for AI players
    if(collector->IsAI())
        return ;

    // emit notification
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

    IsoObject * isoObj = GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY() - isoObj->GetHeight() * 0.25f;

    const float speed = 50.f;
    const float decaySpeed = 150.f;

    DataParticleOutput pd(mNum, OT_DIAMONDS, x0, y0);

    pu->AddParticle(pd);

    // play SFX
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("game/collect-03.ogg");
}

void Diamonds::UpdateGraphics()
{
    SetImage();
}

void Diamonds::SetImage()
{
    // set color
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    // assign texture
    const int spriteId = SpriteCollectiblesId::DIAMONDS_1 + (mNum - 1);

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteCollectiblesFile, spriteId);

    isoObj->SetTexture(tex);
}

void Diamonds::SetObjColors()
{
    mObjColors.clear();

    mObjColors.push_back(0xf9f4b8ff);
    mObjColors.push_back(0xf5ed89ff);
    mObjColors.push_back(0xe1d66bff);
    mObjColors.push_back(0xdbd057ff);
    mObjColors.push_back(0xd7ca42ff);
    mObjColors.push_back(0xbdb128ff);
}

} // namespace game
