#include "WeaponData.h"

#include "GameConstants.h"

namespace game
{

// TYPES
const WeaponType WeaponData::TYPE_LASER1 = 2643265898720201443u;
const WeaponType WeaponData::TYPE_LASER2 = 17367507981825996503u;
const WeaponType WeaponData::TYPE_LASER3 = 8496353577309759083u;
const WeaponType WeaponData::TYPE_LASER4 = 15783589244367080129u;
const WeaponType WeaponData::TYPE_LASER5 = 12405400515080753888u;

const WeaponType WeaponData::TYPE_NULL = 0;

// NULL OBJECT
const WeaponData WeaponData::NullObj(TYPE_NULL, WEAPONC_NULL, 0, 0.f, {},
                                     { {ATT_AIMED_SHOT, 0}, {ATT_BURST_SHOT, 0}, {ATT_QUICK_SHOT, 0} });

WeaponData::WeaponData(WeaponType wt, WeaponClass wc, unsigned int bs, float tc,
                       const std::unordered_map<ObjAttId, int> & atts,
                       const std::unordered_map<AttackMode, int> & ec)
    : mAttributes(atts)
    , mEnergyCosts(ec)
    , mType(wt)
    , mClass(wc)
    , mTimeCooldown(tc)
    , mBurstShots(bs)
{
}

} // namespace game
