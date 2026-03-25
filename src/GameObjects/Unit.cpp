#include "GameObjects/Unit.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/ObjectInitData.h"
#include "GameObjectTools/Weapon.h"
#include "Particles/DataParticleHealing.h"
#include "Particles/UpdaterHealing.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

#include <unordered_map>
#include <cmath>

namespace game
{

Unit::Unit(const ObjectData & data, const ObjectInitData & initData)
    : GameObject(data, initData)
    , mScreen(initData.GetScreen())
    , mStructToBuild(ObjectData::TYPE_NULL)
{
    SetDefaultAction(MOVE);

    // speed
    const float maxSpeed = 5.f;
    SetMaxSpeed(maxSpeed);

    // health
    const float maxHealthValue = 250.f;
    SetMaxHealth(maxHealthValue);

    // init graphics
    SetImage();
}

bool Unit::CanAttack() const
{
    return mWeapon != nullptr;
}

void Unit::ClearTargetAttack()
{
    if(mWeapon != nullptr)
        mWeapon->ClearTarget();
}

bool Unit::IsTargetAttackInRange(const GameObject * obj) const
{
    if(mWeapon != nullptr)
        return mWeapon->IsTargetInRange(obj);
    else
        return false;
}

bool Unit::SetTargetAttack(GameObject * obj)
{
    if(mWeapon != nullptr)
        return mWeapon->SetTarget(obj);
    else
        return false;
}

bool Unit::CanHeal() const
{
    return GetAttribute(OBJ_ATT_HEALING_RANGE) > 0 &&
           GetAttribute(OBJ_ATT_HEALING_POWER) > 0;
}

int Unit::GetHealingRange() const
{
    const int HealRanges[] = { 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4 };
    return HealRanges[GetAttribute(OBJ_ATT_HEALING_RANGE)];
}

float Unit::GetHealingPower() const
{
    const float HealPowers[] = { 0.f, 1.f, 2.f, 2.f, 3.f, 3.f, 4.f, 4.f, 5.f, 5.f, 6.f };
    return HealPowers[GetAttribute(OBJ_ATT_HEALING_POWER)];
}

bool Unit::IsTargetHealingInRange(GameObject * obj) const
{
    const int range = GetHealingRange();

    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            if(std::abs(GetRow0() - r) <= range && std::abs(GetCol0() - c) <= range)
                return true;
        }
    }

    return false;
}

bool Unit::SetTargetHealing(GameObject * obj)
{
    if(nullptr == obj || !IsTargetHealingInRange(obj) || !obj->IsVisible() ||
       obj == this || obj->GetObjectCategory() != ObjectData::CAT_UNIT ||
       obj->IsHealthMax())
        return false;

    mTargetHealing = obj;
    mTimerHealing = 0.f;

    return true;
}

void Unit::Update(float delta)
{
    // ATTACKING OTHER OBJECTS
    if(mWeapon != nullptr)
    {
        if(!mWeapon->Update(delta))
            mScreen->SetObjectActionFailed(this);

        if(mWeapon->IsReadyToShoot())
            PrepareShoot();
    }

    // HEALING OTHER OBJECTS
    if(mTargetHealing)
        UpdateHealing(delta);
}

bool Unit::CanBuild() const
{
    return GetAttribute(OBJ_ATT_CONSTRUCTION) > 0;
}

float Unit::GetTimeBuildStructure() const
{
    const float maxTime = 6.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_CONSTRUCTION));
}

float Unit::GetTimeBuildWall() const
{
    const float maxTime = 3.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_CONSTRUCTION));
}

void Unit::ClearStructureToBuild() { mStructToBuild = ObjectData::TYPE_NULL; }

bool Unit::CanConquer() const
{
    return GetAttribute(OBJ_ATT_CONQUEST) > 0;
}

float Unit::GetTimeConquestCell() const
{
    const float maxTime = 2.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_CONQUEST));
}

float Unit::GetTimeConquestStructure() const
{
    const float maxTime = 5.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_CONQUEST));
}

bool Unit::CanSpawn() const
{
    return GetAttribute(OBJ_ATT_SPAWNING) > 0;
}

float Unit::GetTimeSpawnMiniUnit() const
{
    const float maxTime = 2.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_SPAWNING));
}

void Unit::UpdateGraphics()
{
    SetImage();
}

void Unit::SetImage()
{
    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    const GameObjectTypeId type = GetObjectType();
    const unsigned int ind = TypeToIndex(type);

    const unsigned int texInd = (NUM_UNIT_SPRITES_PER_FACTION * faction) +
                                (NUM_UNIT_SPRITES_PER_TYPE * ind) +
                                 static_cast<unsigned int>(IsSelected());
    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex =tm->GetSprite(SpriteFileUnits, texInd);

    GetIsoObject()->SetTexture(tex);
}

void Unit::UpdateHealing(float delta)
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
                mScreen->SetObjectActionCompleted(this);

                return ;
            }
        }
        else
        {
            mTargetHealing = nullptr;

            // mark healing action as failed
            mScreen->SetObjectActionFailed(this);
        }
    }
    // target destroyed -> stop
    else
    {
        mTargetHealing = nullptr;

        // mark healing action as failed
        mScreen->SetObjectActionFailed(this);
    }

    mTimerHealing = mTimeHealing;
}

void Unit::PrepareShoot()
{
    const IsoObject * isoObj = GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY();

    mWeapon->Shoot(x0, y0);

    if(!mWeapon->HasTarget())
        mScreen->SetObjectActionCompleted(this);
}

void Unit::Heal()
{
    using namespace sgl::graphic;

    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    // consume energy
    ActionStepCompleted(HEAL);

    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterHealing *>(partMan->GetUpdater(PU_HEALING));

    const unsigned int texInd = SpriteIdUnitsParticles::IND_UPAR_HEAL_F1 + faction;
    Texture * tex = TextureManager::Instance()->GetSprite(SpriteFileUnitsParticles, texInd);

    IsoObject * isoObj = GetIsoObject();
    IsoObject * isoTarget = mTargetHealing->GetIsoObject();

    const float x0 = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float y0 = isoObj->GetY();
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
        GetHealingPower(),
        GetFaction()
    };

    pu->AddParticle(pd);
}

unsigned int Unit::TypeToIndex(GameObjectTypeId type)
{
    const std::unordered_map<GameObjectTypeId, unsigned int> indexes =
    {
        { ObjectData::TYPE_UNIT_WORKER1, 0 },
        { ObjectData::TYPE_UNIT_SOLDIER1, 1 },
        { ObjectData::TYPE_UNIT_SOLDIER2, 2 },
        { ObjectData::TYPE_UNIT_SCOUT1, 3 },
        { ObjectData::TYPE_UNIT_MEDIC1, 4 },
        { ObjectData::TYPE_UNIT_SPAWNER1, 5 },
        { ObjectData::TYPE_UNIT_SPAWNER2, 6 },
    };

    return indexes.at(type);
}

GameObjectTypeId Unit::IndexToType(unsigned int ind)
{
    const GameObjectTypeId types[] =
    {
        ObjectData::TYPE_UNIT_WORKER1,
        ObjectData::TYPE_UNIT_SOLDIER1,
        ObjectData::TYPE_UNIT_SOLDIER2,
        ObjectData::TYPE_UNIT_SCOUT1,
        ObjectData::TYPE_UNIT_MEDIC1,
        ObjectData::TYPE_UNIT_SPAWNER1,
        ObjectData::TYPE_UNIT_SPAWNER2,
    };

    return types[ind];
}

} // namespace game
