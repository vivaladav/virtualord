#pragma once

#include <unordered_map>

namespace sgl
{
    namespace graphic { class ParticlesManager; }
    namespace utilities { class BinaryFile; }
}

namespace game
{

class Game;
class GameMap;
class GameObject;
class WeaponData;

enum AttackMode : unsigned int;
enum ObjAttId : unsigned int;

class Weapon
{
public:
    Weapon(const WeaponData & data, GameObject * owner, const Game * g,
           GameMap * gm, const sgl::graphic::ParticlesManager * partMan);
    virtual ~Weapon() = default;

    virtual bool Save(sgl::utilities::BinaryFile & bf) const;

    AttackMode GetAttackMode() const;
    void SetAttackMode(AttackMode am);

    // special methods used by tutorial
    bool IsPerfectShotEnabled() const;
    void SetPerfectShot(bool enabled);
    bool IsFatalHitEnabled() const;
    void SetFatalHit(bool enabled);

    // cost for whole attack (i.e.: all burst shots)
    int GetCostEnergy() const;
    // cost for a single shot of an attack
    int GetCostEnergyPerShot() const;

    void ClearTarget();
    bool HasTarget() const;
    const GameObject * GetTarget() const;
    bool SetTarget(GameObject * obj);
    bool IsTargetInRange(const GameObject * obj) const;

    float GetProbabilityHit(const GameObject * target) const;
    float GetProbabilityFatalHit(const GameObject * target) const;
    float GetMaxProbabilityFatalHit() const;
    void SetMaxProbabilityFatalHit(float val);

    unsigned int GetBurstShots() const;
    unsigned int GetBurstToShoot() const;

    float GetTimeCooldown() const;

    int GetRange() const;

    const std::unordered_map<ObjAttId, int> & GetAttributes() const;

    bool IsReadyToShoot() const;
    void Shoot(float x0, float y0);

    bool Update(float delta);

protected:
    GameObject * GetOwner() const;

    const Game * GetGame() const;
    GameMap * GetGameMap() const;
    const sgl::graphic::ParticlesManager * GetParticlesManager() const;

protected:
    GameObject * mTarget = nullptr;

private:
    float GetBonusHitOnAttackMode(float prob) const;
    float GetBonusFatalHitOnAttackMode(float prob) const;

    virtual void OnShoot(float x0, float y0) = 0;

private:
    std::unordered_map<ObjAttId, int> mAttributes;

    const std::unordered_map<AttackMode, int> mEnergyCosts;

    GameObject * mOwner = nullptr;

    const Game * mGame = nullptr;
    GameMap * mGameMap = nullptr;

    const sgl::graphic::ParticlesManager * mPartMan = nullptr;

    // attack
    AttackMode mAttackMode;
    float mTimerAttack = 0.f;

    int mRange = 0;
    int mBurstShots = 0;
    int mBurstToShoot = 0;
    float mTimeCooldown = 0.f;
    float mMaxProbabilityFatal = 3.f;

    bool mReadyToShoot = false;
    bool mPerfectShot = false;
    bool mFatalHit = true;
};

inline AttackMode Weapon::GetAttackMode() const { return mAttackMode; }
inline void Weapon::SetAttackMode(AttackMode am) { mAttackMode = am; }

inline bool Weapon::IsPerfectShotEnabled() const { return mPerfectShot; }
inline void Weapon::SetPerfectShot(bool enabled) { mPerfectShot = enabled; }

inline bool Weapon::IsFatalHitEnabled() const { return mFatalHit; }
inline void Weapon::SetFatalHit(bool enabled) { mFatalHit = enabled;}

inline void Weapon::ClearTarget() { mTarget = nullptr; }
inline bool Weapon::HasTarget() const { return mTarget != nullptr; }
inline const GameObject * Weapon::GetTarget() const { return mTarget; }

inline float Weapon::GetMaxProbabilityFatalHit() const { return mMaxProbabilityFatal; }
inline void Weapon::SetMaxProbabilityFatalHit(float val) { mMaxProbabilityFatal = val; }

inline unsigned int Weapon::GetBurstShots() const { return mBurstShots; }
inline unsigned int Weapon::GetBurstToShoot() const { return mBurstToShoot; }
inline float Weapon::GetTimeCooldown() const { return mTimeCooldown; }

inline int Weapon::GetRange() const { return mRange; }

inline bool Weapon::IsReadyToShoot() const { return mReadyToShoot; }

inline const std::unordered_map<ObjAttId, int> & Weapon::GetAttributes() const { return mAttributes; }

inline GameObject * Weapon::GetOwner() const { return mOwner; }

inline const Game * Weapon::GetGame() const { return mGame; }
inline GameMap * Weapon::GetGameMap() const { return mGameMap; }

inline const sgl::graphic::ParticlesManager * Weapon::GetParticlesManager() const { return mPartMan; }

} // namespace game
