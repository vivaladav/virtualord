#include "GameObjects/LootBox.h"

#include "GameData.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleLootboxPrize.h"
#include "Particles/UpdaterLootboxPrize.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/LoadedDie.h>
#include <sgl/utilities/UniformDistribution.h>

#include <cmath>

namespace game
{

LootBox::LootBox()
    : Collectable(GameObject::TYPE_LOOTBOX, 1, 1)
{
    SetImage();

    SetObjColors();

    // define TYPE of prize
    sgl::utilities::LoadedDie die({ 15.f, 15.f, 20.f, 20.f, 30.f });
    mPrizeType = static_cast<LootBox::Prize>(die.GetNextValue());

    // define QUANTITY of prize
    int min;
    int max;

    if(LB_MONEY == mPrizeType)
    {
        min = 100;
        max = 500;
    }
    else if(LB_BLOBS == mPrizeType || LB_DIAMONDS == mPrizeType)
    {
        min = 5;
        max = 25;
    }
    // energy and material
    else
    {
        min = 50;
        max = 250;
    }

    sgl::utilities::UniformDistribution d(min, max);

    // round quantity to 5
    const int r = 5;
    mPrizeQuantity = std::roundf(d.GetNextValue() / static_cast<float>(r)) * r;
}

void LootBox::Collected(Player * collector)
{
    Collectable::Collected(collector);

    // do not show anyting for AI players
    if(collector->IsAI())
        return ;

    // emit notification
    auto pu = static_cast<UpdaterLootboxPrize *>(GetScreen()->GetParticleUpdater(PU_LOOTBOX_PRIZE));

    IsoObject * isoObj = GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY() - isoObj->GetHeight() * 0.25f;

    const float speed = 50.f;
    const float decaySpeed = 150.f;

    DataParticleLootboxPrize pd(mPrizeQuantity, mPrizeType, x0, y0, speed, decaySpeed);

    pu->AddParticle(pd);
}

void LootBox::UpdateGraphics()
{
    SetImage();

    SetObjColors();
}

void LootBox::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int sel = static_cast<unsigned int>(IsSelected());
    const unsigned int texInd = IND_LOOTBOX + sel;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteCollectiblesFile, texInd);
    isoObj->SetTexture(tex);
}

void LootBox::SetObjColors()
{
    mObjColors.clear();

    mObjColors.push_back(0xdba457ff);
    mObjColors.push_back(0xd28d2dff);
    mObjColors.push_back(0xe0b06cff);
    mObjColors.push_back(0x805519ff);
    mObjColors.push_back(0x6b4715ff);
    mObjColors.push_back(0x95631dff);
}

} // namespace game
