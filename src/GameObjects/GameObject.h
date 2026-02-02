#pragma once

#include "GameObjectTypes.h"
#include "GameObjects/ObjectData.h"

#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

namespace sgl
{
    namespace graphic { class ParticlesManager; }
}

namespace game
{

enum GameObjectActionType : unsigned int
{
    IDLE,

    // BASE
    BUILD_UNIT,

    // MINI UNIT
    SET_TARGET,

    // UNIT
    MOVE,
    CONQUER_CELL,
    CONQUER_STRUCTURE,
    ATTACK,
    BUILD_STRUCTURE,
    BUILD_WALL,
    HEAL,
    SPAWN,

    // WALL GATE
    TOGGLE_GATE,

    // GENERIC
    SELF_DESTRUCTION,

    NUM_OBJ_ACTIONS
};

class Game;
class GameMap;
class GameObjectsGroup;
class IconUpgrade;
class IsoObject;
class ObjectInitData;
class Player;
class WarningMessage;
class Weapon;
struct GameMapCell;

enum PlayerFaction : unsigned int;

class GameObject
{
public:
    // -- OBJECT VARIANT --
    static const GameObjectVariantId VAR_0;

    static const int MAX_LEVEL = 10;
    static const int UPGRADE_POINTS[MAX_LEVEL];

public:
    GameObject(const ObjectData & data, const ObjectInitData & initData);
    virtual ~GameObject();

    // GROUP
    bool IsInGroup() const;
    GameObjectsGroup * GetGroup() const;
    void SetGroup(GameObjectsGroup * g);
    void ClearGroup();

    void SetOwner(Player * p);

    unsigned int GetObjectId() const;

    IsoObject * GetIsoObject() const;
    virtual void OnPositionChanged();

    int GetX() const;
    int GetY() const;
    void SetPosition(int x, int y);
    void SetX(int x);
    void SetY(int y);

    void ShowWarning(const char * text, float time);

    bool IsStructure() const;
    bool CanBeConquered() const;

    bool CanBeCollected() const;

    bool IsSelected() const;
    void SetSelected(bool val);

    bool IsBusy() const;

    bool IsVisited() const;
    void SetVisited();

    bool IsVisible() const;
    void SetVisible(bool val);

    bool IsLinked() const;
    void SetLinked(bool val);

    bool IsStatic() const;

    bool IsDestroyed() const;

    const GameMapCell * GetCell() const;
    void SetCell(const GameMapCell * cell);

    int GetVisibilityLevel() const;

    int GetRow0() const;
    int GetCol0() const;
    int GetRow1() const;
    int GetCol1() const;

    unsigned int GetRows() const;
    unsigned int GetCols() const;

    PlayerFaction GetFaction() const;
    bool SetFaction(PlayerFaction f);
    bool IsFactionLocal() const;

    GameObjectTypeId GetObjectType() const;
    GameObjectCategoryId GetObjectCategory() const;
    GameObjectVariantId GetObjectVariant() const;
    void SetObjectVariant(GameObjectVariantId var);

    bool IsHealthMax() const;
    float GetHealth() const;
    float GetMaxHealth() const;
    void SumHealth(float val);

    bool IsEnergyMax() const;
    float GetEnergy() const;
    void SumEnergy(float val);
    float GetMaxEnergy() const;

    float GetEnergyForActionStep(GameObjectActionType action) const;
    bool HasEnergyForActionStep(GameObjectActionType action) const;
    void ActionStepCompleted(GameObjectActionType action);

    int GetExperience() const;
    int GetExperienceToLevel(int level) const;
    int GetExperienceToNextLevel() const;
    int GetExperienceLevel() const;
    void SumExperience(int val);
    bool CanBeUpgraded() const;
    void UpgradeLevel(const std::vector<int> & attChanges);

    unsigned int AddFunctionOnValueChanged(const std::function<void()> & f);
    void RemoveFunctionOnValueChanged(unsigned int fId);

    // ATTRIBUTES
    int GetAttribute(ObjAttId attID) const;
    const std::unordered_map<ObjAttId, int> & GetAttributes() const;

    float GetSpeed() const;

    // WEAPON
    const Weapon * GetWeapon() const;
    void SetWeapon(Weapon * w);

    void SetAttackMode(AttackMode am);

    void FindAndSetEnemyTarget();
    bool HasEnemyInRange();

    void Hit(float damage, GameObject * attacker, bool fatal, bool showHitPoints = true);
    void MissHit();
    void SelfDestroy();

    GameObjectActionType GetActiveAction() const;
    void SetActiveAction(GameObjectActionType action);
    void SetActiveActionToDefault();
    GameObjectActionType GetCurrentAction() const;
    void SetCurrentAction(GameObjectActionType action);
    GameObjectActionType GetDefaultAction() const;
    void SetDefaultAction(GameObjectActionType action);

    virtual void OnNewTurn(PlayerFaction faction);

    virtual void Update(float delta);

protected:
    virtual void UpdateGraphics() = 0;

    virtual void OnFactionChanged();
    virtual void OnLinkedChanged();
    virtual void OnAttributeChanged();

    const Game * GetGame() const;
    GameMap * GetGameMap() const;
    const sgl::graphic::ParticlesManager * GetParticlesManager() const;
    Player * GetOwner() const;

    void SetStructure(bool val);
    void SetCanBeConquered(bool val);
    void SetStatic(bool val);

    void SetMaxSpeed(float val);

    void NotifyValueChanged();

    float GetTime(float maxTime, float attribute) const;

    void SetMaxVisibilityLevel(float maxVal, float maxValLinked);
    void SetMaxEnergy(float val);
    void SetMaxHealth(float maxVal);
    float GetRegenerationPower() const;

protected:
    static const unsigned int COLOR_FOW;
    static const unsigned int COLOR_VIS;

    std::vector<unsigned int> mObjColors;

    GameObjectVariantId mVariant = VAR_0;

    Weapon * mWeapon = nullptr;

private:
    // NOTE no boundary check, assuming valid action
    virtual float GetActionEnergyCost(GameObjectActionType action) const;
    // NOTE no boundary check, assuming valid action
    virtual float GetActionExperienceGain(GameObjectActionType action) const;

    void PositionWarningMessage();

    void ShowIconUpgrade();
    void HideIconUpgrade();
    void PositionIconUpgrade();

    void SetEnergy(float val);
    void SetExperience(int val);

    void SetHealth(float val);

    void RestoreTurnEnergy();

    void SetDefaultColors();

private:
    static unsigned int counter;

private:
    std::map<unsigned int, std::function<void()>> mOnValueChanged;

    std::unordered_map<ObjAttId, int> mAttributes;

    GameObjectsGroup * mGroup = nullptr;

    IsoObject * mIsoObj = nullptr;

    unsigned int mObjId;

    const Game * mGame = nullptr;
    GameMap * mGameMap = nullptr;
    const sgl::graphic::ParticlesManager * mPartMan = nullptr;
    Player * mOwner = nullptr;

    const GameMapCell * mCell = nullptr;

    WarningMessage * mWarnMessage = nullptr;

    IconUpgrade * mIconUpgrade = nullptr;

    GameObjectTypeId mType = ObjectData::TYPE_NULL;
    GameObjectCategoryId mCategory = ObjectData::CAT_NULL;

    GameObjectActionType mActiveAction = IDLE;
    GameObjectActionType mCurrAction = IDLE;
    GameObjectActionType mDefaultAction = IDLE;

    PlayerFaction mFaction;

    unsigned int mRows = 1;
    unsigned int mCols = 1;

    int mMaxVisLevel = 0;

    int mExpLevel = 0;
    int mExp = 0;

    float mMaxEnergy = 100.f;
    float mEnergy = 100.f;
    float mMaxHealth = 100.f;
    float mHealth = 100.f;
    float mMaxSpeed = 0.f;

    bool mStructure = false;
    // set to TRUE for elements that stay visible under FOGOW
    bool mStatic = false;

    bool mCanBeConq = false;

    bool mSelected = false;

    bool mVisible = true;
    bool mVisited = false;
    bool mLinked = false;
};

inline bool GameObject::IsInGroup() const { return mGroup != nullptr; }
inline GameObjectsGroup * GameObject::GetGroup() const { return mGroup; }

inline unsigned int GameObject::GetObjectId() const { return mObjId; }

inline IsoObject * GameObject::GetIsoObject() const { return mIsoObj; }

inline bool GameObject::IsStructure() const { return mStructure; }

inline bool GameObject::CanBeConquered() const { return mCanBeConq; }

inline bool GameObject::CanBeCollected() const { return ObjectData::CAT_COLLECTABLE == mCategory; }

inline bool GameObject::IsSelected() const { return mSelected; }

inline bool GameObject::IsBusy() const { return mCurrAction != IDLE; }

inline bool GameObject::IsVisited() const { return mVisited; }
inline void GameObject::SetVisited() { mVisited = true; }

inline bool GameObject::IsVisible() const { return mVisible; }

inline bool GameObject::IsLinked() const { return mLinked; }

inline bool GameObject::IsStatic() const { return mStatic; }

inline bool GameObject::IsDestroyed() const
{
    const float minH = 0.01f;
    return mHealth < minH;
}

inline const GameMapCell * GameObject::GetCell() const { return mCell; }

inline GameObjectTypeId GameObject::GetObjectType() const { return mType; }

inline GameObjectCategoryId GameObject::GetObjectCategory() const { return mCategory; }

inline GameObjectVariantId GameObject::GetObjectVariant() const { return mVariant; }

inline unsigned int GameObject::GetRows() const { return mRows; }
inline unsigned int GameObject::GetCols() const { return mCols; }

inline PlayerFaction GameObject::GetFaction() const { return mFaction; }

inline const Game * GameObject::GetGame() const { return mGame; }
inline GameMap * GameObject::GetGameMap() const { return mGameMap; }
inline const sgl::graphic::ParticlesManager * GameObject::GetParticlesManager() const { return mPartMan; }
inline Player * GameObject::GetOwner() const { return mOwner; }

inline void GameObject::SetStructure(bool val) { mStructure = val; }
inline void GameObject::SetCanBeConquered(bool val) { mCanBeConq = val; }
inline void GameObject::SetStatic(bool val) { mStatic = val; }

inline void GameObject::SetMaxSpeed(float val) { mMaxSpeed = val; }

inline float GameObject::GetHealth() const { return mHealth; }

inline float GameObject::GetEnergy() const { return mEnergy; }

inline float GameObject::GetEnergyForActionStep(GameObjectActionType action) const
{
    if(action < NUM_OBJ_ACTIONS)
        return GetActionEnergyCost(action);
    else
        return 0.f;
}

inline int GameObject::GetExperience() const { return mExp; }
inline int GameObject::GetExperienceToNextLevel() const
{
    return GetExperienceToLevel(mExpLevel + 1);
}
inline int GameObject::GetExperienceLevel() const { return mExpLevel; }
inline bool GameObject::CanBeUpgraded() const
{
    return mExp >= GetExperienceToNextLevel() && mExpLevel < MAX_LEVEL;
}

inline const std::unordered_map<ObjAttId, int> & GameObject::GetAttributes() const
{
    return mAttributes;
}

inline const Weapon * GameObject::GetWeapon() const { return mWeapon; }

inline GameObjectActionType GameObject::GetActiveAction() const { return mActiveAction; }
inline void GameObject::SetActiveAction(GameObjectActionType action) { mActiveAction = action; }
inline GameObjectActionType GameObject::GetCurrentAction() const { return mCurrAction; }
inline void GameObject::SetCurrentAction(GameObjectActionType action) { mCurrAction = action; }
inline GameObjectActionType GameObject::GetDefaultAction() const { return mDefaultAction; }
inline void GameObject::SetDefaultAction(GameObjectActionType action) { mDefaultAction = action; }

inline void GameObject::SetMaxEnergy(float val)
{
    mMaxEnergy = val;
    SetEnergy(val);
}

inline void GameObject::SetMaxHealth(float val)
{
    mMaxHealth = val;
    SetHealth(val);
}

} // namespace game
