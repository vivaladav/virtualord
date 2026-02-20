#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <unordered_map>

namespace game
{

enum WeaponClass : unsigned int
{
    WEAPONC_LASER,

    NUM_WEAPON_CLASSES,

    WEAPONC_NULL
};

enum AttackMode : unsigned int;
enum ObjAttId : unsigned int;

class WeaponData
{
public:
    static const WeaponType TYPE_LASER1;
    static const WeaponType TYPE_LASER2;
    static const WeaponType TYPE_LASER3;
    static const WeaponType TYPE_LASER4;
    static const WeaponType TYPE_LASER5;

    static const WeaponType TYPE_NULL;
    static const WeaponData NullObj;

public:
    WeaponData(WeaponType wt, WeaponClass wc, unsigned int bs, float tc,
               const std::unordered_map<ObjAttId, int> & atts,
               const std::unordered_map<AttackMode, int> & ec);

    WeaponType GetType() const;
    WeaponClass GetClass() const;

    const std::unordered_map<ObjAttId, int> & GetAttributes() const;

    const std::unordered_map<AttackMode, int> & GetEnergyCosts() const;

    unsigned int GetBurstShots() const;
    float GetTimeCooldown() const;

private:
    const std::unordered_map<ObjAttId, int> mAttributes;
    const std::unordered_map<AttackMode, int> mEnergyCosts;

    WeaponType mType;
    WeaponClass mClass;

    float mTimeCooldown;
    int mBurstShots;
};

inline WeaponType WeaponData::GetType() const { return mType; }
inline WeaponClass WeaponData::GetClass() const { return mClass; }

inline const std::unordered_map<ObjAttId, int> & WeaponData::GetAttributes() const
{
    return mAttributes;
}

inline const std::unordered_map<AttackMode, int> & WeaponData::GetEnergyCosts() const
{
    return mEnergyCosts;
}

inline unsigned int WeaponData::GetBurstShots() const { return mBurstShots; }
inline float WeaponData::GetTimeCooldown() const { return mTimeCooldown; }

} // namespace game
