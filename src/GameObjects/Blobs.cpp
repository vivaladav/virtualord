#include "GameObjects/Blobs.h"

#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleOutput.h"
#include "Particles/UpdaterOutput.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace
{
constexpr unsigned int MIN_UNITS = 1;
constexpr unsigned int MAX_UNITS = 4;
}

namespace game
{

Blobs::Blobs(const ObjectData & data, const ObjectInitData & initData)
    : Collectable(data, initData, MIN_UNITS, MAX_UNITS)
{
    SetObjColors();
    SetImage();
}

void Blobs::Collected(Player * collector)
{
    // do not show anyting for AI players
    if(collector->IsAI())
        return ;

    // emit notification
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterOutput *>(partMan->GetUpdater(PU_OUTPUT));

    IsoObject * isoObj = GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY() - isoObj->GetHeight() * 0.25f;

    DataParticleOutput pd(GetNumUnits(), OT_BLOBS, x0, y0);

    pu->AddParticle(pd);

    // play SFX
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("game/collect-02.ogg");
}

void Blobs::UpdateGraphics()
{
    SetImage();
}

void Blobs::SetImage()
{
    // set color
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    // assign texture
    const int spriteId = SpriteCollectiblesId::BLOBS_1 + (GetNumUnits() - 1);

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteCollectiblesFile, spriteId);

    isoObj->SetTexture(tex);
}

void Blobs::SetObjColors()
{
    mObjColors.clear();

    mObjColors.push_back(0xf9b8f9ff);
    mObjColors.push_back(0xf589f5ff);
    mObjColors.push_back(0xe16be1ff);
    mObjColors.push_back(0xdb57dbff);
    mObjColors.push_back(0xd742d7ff);
    mObjColors.push_back(0xd22dd2ff);
}

} // namespace game
