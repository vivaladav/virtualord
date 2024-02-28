#include "GameObjects/Hospital.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "ObjectData.h"
#include "GameMap.h"
#include "Particles/DataParticleHealing.h"
#include "Particles/UpdaterHealing.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

Hospital::Hospital(const ObjectFactionData & facData)
    : Structure(GameObject::TYPE_HOSPITAL, GameObject::CAT_GENERIC, 2, 2)
{
    // SET STATS values in range [1-10]
    mAttributes.resize(facData.stats.size());
    mAttributes = facData.stats;

    // set healing range converting attribute
    const int maxHealVal = 11;
    const int HealRanges[maxHealVal] = { 0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6 };
    mRangeHealing = HealRanges[mAttributes[OSTAT_HEALING]];

    // set healing power converting attribute
    const float HealPowers[maxHealVal] = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f };
    mHealingPower = HealPowers[mAttributes[OSTAT_HEALING]];

    SetVisibilityLevel(4);
}

bool Hospital::IsTargetHealingInRange(GameObject * obj) const
{
    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            if(std::abs(GetRow0() - r) <= mRangeHealing && std::abs(GetCol0() - c) <= mRangeHealing)
                return true;
        }
    }

    return false;
}

bool Hospital::SetTargetHealing(GameObject * obj)
{
    if(nullptr == obj || !IsTargetHealingInRange(obj) || !obj->IsVisible() ||
       obj == this || obj->GetObjectCategory() != GameObject::CAT_UNIT ||
       obj->IsHealthMax())
        return false;

    mTargetHealing = obj;
    mTimerHealing = 0.f;

    return true;
}

void Hospital::Update(float delta)
{
    // HEALING OTHER OBJECTS
    if(mTargetHealing)
        UpdateHealing(delta);
}

void Hospital::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void Hospital::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_HOSPITAL;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_HOSPITAL + sel;
    else
        texInd = ID_STRUCT_HOSPITAL_F1 + (faction * NUM_HOSPITAL_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}


void Hospital::UpdateHealing(float delta)
{
    mTimerHealing -= delta;

    // not healing yet...
    if(mTimerHealing > 0.f)
        return ;

    // target still alive -> try to heal
    if(GetGameMap()->HasObject(mTargetHealing))
    {
        if(IsTargetHealingInRange(mTargetHealing))
        {
            Heal();

            // finished healing
            if(mTargetHealing->GetHealth() >= mTargetHealing->GetMaxHealth())
            {
                mTargetHealing = nullptr;

                // mark healing action as completed
                GetScreen()->SetObjectActionCompleted(this);

                return ;
            }
        }
        else
        {
            mTargetHealing = nullptr;

            // mark healing action as failed
            GetScreen()->SetObjectActionFailed(this);
        }
    }
    // target destroyed -> stop
    else
    {
        mTargetHealing = nullptr;

        // mark healing action as failed
        GetScreen()->SetObjectActionFailed(this);
    }

    mTimerHealing = mTimeHealing;
}

void Hospital::Heal()
{
    using namespace sgl::graphic;

    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    // consume energy
    ActionStepCompleted(HEAL);

    auto pu = static_cast<UpdaterHealing *>(GetScreen()->GetParticleUpdater(PU_HEALING));

    const unsigned int texInd = SpriteIdUnitsParticles::IND_UPAR_HEAL_F1 + faction;
    Texture * tex = TextureManager::Instance()->GetSprite(SpriteFileUnitsParticles, texInd);

    IsoObject * isoObj = GetIsoObject();
    IsoObject * isoTarget = mTargetHealing->GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY() + isoObj->GetHeight() * 0.3f;
    const float tX = isoTarget->GetX() + (isoTarget->GetWidth() - tex->GetWidth()) * 0.5f;
    const float tY = isoTarget->GetY() + (isoTarget->GetHeight() - tex->GetHeight()) * 0.5f;
    const float speed = 100.f;

    const DataParticleHealing pd =
        {
            tex,
            GetGameMap(),
            mTargetHealing,
            x0,
            y0,
            tX,
            tY,
            speed,
            mHealingPower,
            GetFaction()
        };

    pu->AddParticle(pd);
}

} // namespace game
