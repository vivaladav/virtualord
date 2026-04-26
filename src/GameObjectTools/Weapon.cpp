#include "GameObjectTools/Weapon.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "GameObjects/GameObject.h"
#include "GameObjectTools/WeaponData.h"

#include <cstdlib>

namespace game
{

Weapon::Weapon(const WeaponData & data, GameObject * owner, const Game * g,
               GameMap * gm, const sgl::graphic::ParticlesManager * partMan)
    : mAttributes(data.GetAttributes())
    , mEnergyCosts(data.GetEnergyCosts())
    , mOwner(owner)
    , mGame(g)
    , mGameMap(gm)
    , mPartMan(partMan)
    , mAttackMode(ATT_QUICK_SHOT)
    , mBurstShots(data.GetBurstShots())
    , mBurstToShoot(data.GetBurstShots())
    , mTimeCooldown(data.GetTimeCooldown())
{
    // set attack range converting attribute
    const int attRanges[] = { 0, 2, 3, 4, 5, 6, 8, 9, 10, 11, 13 };
    mRange = attRanges[mAttributes[OBJ_ATT_ATTACK_RANGE]];
}

int Weapon::GetCostEnergy() const
{
    const int costSingleShot = GetCostEnergyPerShot();

    if(GetAttackMode() == ATT_BURST_SHOT)
        return costSingleShot * GetBurstShots();
    else
        return costSingleShot;
}

int Weapon::GetCostEnergyPerShot() const
{
    auto it = mEnergyCosts.find(mAttackMode);

    if(it != mEnergyCosts.end())
        return it->second;
    else
        return 0;
}

bool Weapon::SetTarget(GameObject * obj)
{
    if(nullptr == obj || !IsTargetInRange(obj) || !obj->IsVisible() ||
       !mOwner->HasEnergyForActionStep(ATTACK) || obj == mOwner)
        return false;

    mTarget = obj;

    mReadyToShoot = false;

    mBurstToShoot = mBurstShots;

    return true;
}

bool Weapon::IsTargetInRange(const GameObject * obj) const
{
    const int r0 = mOwner->GetRow0();
    const int c0 = mOwner->GetCol0();

    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            if(std::abs(r0 - r) <= mRange && std::abs(c0 - c) <= mRange)
                return true;
        }
    }

    return false;
}

float Weapon::GetProbabilityHit(const GameObject * target) const
{
    const int maxDist = mRange * 2;
    const int dist = mGameMap->Distance(mOwner, target);
    const int targetSize = target->GetRows() * target->GetCols();

    const float maxProb = 100.f;
    // base probability is higher when closer to target up to 100% if next to it
    const float baseProb = (maxDist - dist + 1) * maxProb / static_cast<float>(maxDist);
    // fixed probability is part of base that's at least 50% and higher as target gets bigger
    const float fixedW = 0.5f + (0.5f - (0.5f / static_cast<float>(targetSize)));
    const float fixedProb = baseProb * fixedW;

    const float variableProb = (baseProb - fixedProb) * mAttributes.at(OBJ_ATT_ATTACK_ACCURACY) / MAX_STAT_FVAL;
    const float finalProb = fixedProb + variableProb;

    // bonus/malus based on attack mode
    float bonusProb = GetBonusHitOnAttackMode(finalProb);
    const float retProb = finalProb + bonusProb;

    // clamp
    if(retProb < 0.f)
    {
        const float minProb = 1.f;
        return minProb;
    }
    else if(retProb > maxProb)
        return maxProb;
    else
        return retProb;
}

float Weapon::GetProbabilityFatalHit(const GameObject * target) const
{
    const float maxMult = 1.f;

    // distance (higher -> lower chance)
    const float weightDistance = 0.33f;
    const int maxDist = mRange * 2;
    const int dist = mGameMap->Distance(mOwner, target);
    const float multDist = maxMult - (dist / static_cast<float>(maxDist));

    // target's health (higher -> lower chance)
    const float weightHealth = 0.33f;
    const float multHealth = maxMult - (target->GetHealth() / target->GetMaxHealth());

    // accuracy (higher -> higher chance)
    const float weightAccuracy = 0.34f;
    const float multAccuracy = mAttributes.at(OBJ_ATT_ATTACK_ACCURACY) / MAX_STAT_FVAL;

    // probability
    const float finalProb = (mMaxProbabilityFatal * weightDistance * multDist) +
                            (mMaxProbabilityFatal * weightHealth * multHealth) +
                            (mMaxProbabilityFatal * weightAccuracy * multAccuracy);

    // bonus/malus based on attack mode
    const float bonusProb = GetBonusFatalHitOnAttackMode(finalProb);
    const float retProb = finalProb + bonusProb;

    // clamp
    const float maxProb = 100.f;

    if(retProb < 0.f)
    {
        const float minProb = 0.1f;
        return minProb;
    }
    else if(retProb > maxProb)
        return maxProb;
    else
        return retProb;
}

void Weapon::Shoot(float x0, float y0)
{
    OnShoot(x0, y0);

    // reset attack timer
    mTimerAttack = mTimeCooldown;

    // handle burst shots
    if(mAttackMode == ATT_BURST_SHOT)
    {
        --mBurstToShoot;

        if(mBurstToShoot == 0)
            mTarget = nullptr;
    }
    // clear target for single shots
    else
        mTarget = nullptr;

    mReadyToShoot = false;
}

// returns TRUE when owner needs to shoot
bool Weapon::Update(float delta)
{
    mTimerAttack -= delta;

    // nothing to do
    if(mTarget == nullptr)
        return true;

    // FAIL target already destroyed
    if(!mGameMap->HasObject(mTarget))
    {
        mTarget = nullptr;
        return false;
    }

    // cooldown time not over yet
    if(mTimerAttack > 0.f)
        return true;

    // all good -> shoot!
    mReadyToShoot = true;

    return true;
}

float Weapon::GetBonusHitOnAttackMode(float prob) const
{
    if(mAttackMode == ATT_QUICK_SHOT)
    {
        const float bonus = -0.05;
        return prob * bonus;
    }
    else if(mAttackMode == ATT_BURST_SHOT)
    {
        const float bonus = -0.1;
        return prob * bonus;
    }
    else if(mAttackMode == ATT_AIMED_SHOT)
    {
        const float bonus = 0.2;
        return prob * bonus;
    }
    else if(mAttackMode == ATT_PERFECT_SHOT)
        return 100.f;
    else
        return 0.f;
}

float Weapon::GetBonusFatalHitOnAttackMode(float prob) const
{
    if(mAttackMode == ATT_QUICK_SHOT)
    {
        const float bonus = -0.1;
        return prob * bonus;
    }
    else if(mAttackMode == ATT_BURST_SHOT)
    {
        const float bonus = -0.2;
        return prob * bonus;
    }
    else if(mAttackMode == ATT_AIMED_SHOT)
    {
        const float bonus = 0.25;
        return prob * bonus;
    }
    else
        return 0.f;
}

} // namespace game
