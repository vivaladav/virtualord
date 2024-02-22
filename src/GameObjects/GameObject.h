#pragma once

#include "GameObjectTypes.h"

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace game
{

enum GameObjectActionType : unsigned int
{
    IDLE,

    // BASE
    BUILD_UNIT,

    // UNIT
    MOVE,
    CONQUER_CELL,
    CONQUER_STRUCTURE,
    ATTACK,
    BUILD_STRUCTURE,
    BUILD_WALL,
    HEAL,

    // WALL GATE
    TOGGLE_GATE,

    NUM_OBJ_ACTIONS
};

class GameMap;
class IsoObject;
class Player;
class ScreenGame;
struct GameMapCell;

enum PlayerFaction : unsigned int;

class GameObject
{
public:
    // -- OBJECT TYPE --
    static const GameObjectTypeId TYPE_NULL;

    static const GameObjectTypeId TYPE_BASE;
    static const GameObjectTypeId TYPE_BASE_SPOT;
    static const GameObjectTypeId TYPE_BLOBS;
    static const GameObjectTypeId TYPE_DEFENSIVE_TOWER;
    static const GameObjectTypeId TYPE_DIAMONDS;
    static const GameObjectTypeId TYPE_LOOTBOX;
    static const GameObjectTypeId TYPE_MOUNTAINS;
    static const GameObjectTypeId TYPE_PRACTICE_TARGET;
    static const GameObjectTypeId TYPE_RADAR_STATION;
    static const GameObjectTypeId TYPE_RADAR_TOWER;
    static const GameObjectTypeId TYPE_RES_GEN_ENERGY;
    static const GameObjectTypeId TYPE_RES_GEN_ENERGY_SOLAR;
    static const GameObjectTypeId TYPE_RES_GEN_MATERIAL;
    static const GameObjectTypeId TYPE_RES_GEN_MATERIAL_EXTRACT;
    static const GameObjectTypeId TYPE_RES_STORAGE_BLOBS;
    static const GameObjectTypeId TYPE_RES_STORAGE_DIAMONDS;
    static const GameObjectTypeId TYPE_RES_STORAGE_ENERGY;
    static const GameObjectTypeId TYPE_RES_STORAGE_MATERIAL;
    static const GameObjectTypeId TYPE_ROCKS;
    static const GameObjectTypeId TYPE_TEMPLE;
    static const GameObjectTypeId TYPE_TREES;
    static const GameObjectTypeId TYPE_UNIT_MEDIC1;
    static const GameObjectTypeId TYPE_UNIT_SCOUT1;
    static const GameObjectTypeId TYPE_UNIT_SOLDIER1;
    static const GameObjectTypeId TYPE_UNIT_SOLDIER2;
    static const GameObjectTypeId TYPE_UNIT_WORKER1;
    static const GameObjectTypeId TYPE_WALL;
    static const GameObjectTypeId TYPE_WALL_GATE;

    static const std::unordered_map<GameObjectTypeId, std::string> TITLES;
    static const std::unordered_map<GameObjectTypeId, std::string> DESCRIPTIONS;

    static const unsigned int NUM_UNIT_TYPES = 4;

    // -- OBJECT CATEGORY --
    static const GameObjectCategoryId CAT_NULL;

    static const GameObjectCategoryId CAT_COLLECTABLE;
    static const GameObjectCategoryId CAT_GENERIC;
    static const GameObjectCategoryId CAT_RES_GENERATOR;
    static const GameObjectCategoryId CAT_RES_STORAGE;
    static const GameObjectCategoryId CAT_SCENE_OBJ;
    static const GameObjectCategoryId CAT_UNIT;

    // -- OBJECT VARIANT --
    static const GameObjectVariantId VAR_0;

    static std::string GetObjectTypeStr(const GameObjectTypeId type);

public:
    GameObject(GameObjectTypeId type, GameObjectCategoryId cat, int rows, int cols);
    virtual ~GameObject();

    void SetGameMap(GameMap * map);
    void SetScreen(ScreenGame * screen);

    unsigned int GetObjectId() const;

    IsoObject * GetIsoObject() const;

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
    void SetFaction(PlayerFaction f);

    GameObjectTypeId GetObjectType() const;
    std::string GetObjectTypeStr() const;
    GameObjectCategoryId GetObjectCategory() const;
    GameObjectVariantId GetObjectVariant() const;
    void SetObjectVariant(GameObjectVariantId var);

    float GetHealth() const;
    float GetMaxHealth() const;
    void SetMaxHealth(float max);
    void SetHealth(float val);
    void SumHealth(float val);

    float GetEnergy() const;
    void SetEnergy(float val);
    void SumEnergy(float val);
    float GetMaxEnergy() const;
    void SetMaxEnergy(float val);

    float GetEnergyForActionStep(GameObjectActionType action) const;
    bool HasEnergyForActionStep(GameObjectActionType action) const;
    void ActionStepCompleted(GameObjectActionType action);

    int GetExperience() const;
    int GetExperienceToNextLevel() const;
    int GetExperienceLevel() const;
    int GetMaxExperienceLevel() const;
    void SetExperience(int val);
    void SumExperience(int val);

    unsigned int AddFunctionOnValueChanged(const std::function<void()> & f);
    void RemoveFunctionOnValueChanged(unsigned int fId);

    float GetSpeed() const;

    void Hit(float damage, PlayerFaction attacker);

    GameObjectActionType GetActiveAction() const;
    void SetActiveAction(GameObjectActionType action);
    virtual void SetActiveActionToDefault();
    GameObjectActionType GetCurrentAction() const;
    void SetCurrentAction(GameObjectActionType action);

    virtual void OnNewTurn(PlayerFaction faction);

    virtual void Update(float delta);

protected:
    virtual void UpdateGraphics() = 0;

    virtual void OnLinkedChanged();

    void SetDefaultColors();

    GameMap * GetGameMap() const;
    ScreenGame * GetScreen() const;

    void SetVisibilityLevel(int val);
    void SetStructure(bool val);
    void SetCanBeConquered(bool val);
    void SetStatic(bool val);

    void SetSpeed(float speed);

    void NotifyValueChanged();

protected:
    static const unsigned int COLOR_FOW;
    static const unsigned int COLOR_VIS;

    std::vector<unsigned int> mObjColors;

    GameObjectVariantId mVariant = VAR_0;

private:
    void RestoreTurnEnergy();

private:
    static unsigned int counter;

    static const std::string TYPE_STR_BASE;
    static const std::string TYPE_STR_BASE_SPOT;
    static const std::string TYPE_STR_BLOBS;
    static const std::string TYPE_STR_DEFENSIVE_TOWER;
    static const std::string TYPE_STR_DIAMONDS;
    static const std::string TYPE_STR_LOOTBOX;
    static const std::string TYPE_STR_MOUNTAINS;
    static const std::string TYPE_STR_PRACTICE_TARGET;
    static const std::string TYPE_STR_RADAR_STATION;
    static const std::string TYPE_STR_RADAR_TOWER;
    static const std::string TYPE_STR_RES_GEN_ENERGY;
    static const std::string TYPE_STR_RES_GEN_ENERGY_SOLAR;
    static const std::string TYPE_STR_RES_GEN_MATERIAL;
    static const std::string TYPE_STR_RES_GEN_MATERIAL_EXTRACT;
    static const std::string TYPE_STR_RES_STORAGE_BLOBS;
    static const std::string TYPE_STR_RES_STORAGE_DIAMONDS;
    static const std::string TYPE_STR_RES_STORAGE_ENERGY;
    static const std::string TYPE_STR_RES_STORAGE_MATERIAL;
    static const std::string TYPE_STR_ROCKS;
    static const std::string TYPE_STR_TEMPLE;
    static const std::string TYPE_STR_TREES;
    static const std::string TYPE_STR_UNIT_MEDIC1;
    static const std::string TYPE_STR_UNIT_SCOUT1;
    static const std::string TYPE_STR_UNIT_SOLDIER1;
    static const std::string TYPE_STR_UNIT_SOLDIER2;
    static const std::string TYPE_STR_UNIT_WORKER1;
    static const std::string TYPE_STR_WALL;
    static const std::string TYPE_STR_WALL_GATE;

    static const std::unordered_map<GameObjectTypeId, std::string> TYPE_STR_MAP;

    static const float ACTION_COSTS[NUM_OBJ_ACTIONS];
    static const int ACTION_EXPERIENCE[NUM_OBJ_ACTIONS];

private:
    std::map<unsigned int, std::function<void()>> mOnValueChanged;

    IsoObject * mIsoObj = nullptr;

    unsigned int mObjId;

    GameMap * mGameMap = nullptr;
    ScreenGame * mScreen = nullptr;

    const GameMapCell * mCell = nullptr;

    PlayerFaction mFaction;
    GameObjectTypeId mType = TYPE_NULL;
    GameObjectCategoryId mCategory = CAT_NULL;

    GameObjectActionType mActiveAction = IDLE;
    GameObjectActionType mCurrAction = IDLE;

    unsigned int mRows = 1;
    unsigned int mCols = 1;

    int mVisLevel = 0;

    int mExpLevel = 0;
    int mExp = 0;
    int mExpToNextLvl = 1000;

    float mMaxEnergy = 100.f;
    float mEnergy = 100.f;
    float mEnergyRegPower = 100.f;
    float mMaxHealth = 100.f;
    float mHealth = 100.f;
    float mSpeed = 0.f;

    bool mStructure = false;
    // set to TRUE for elements that stay visible under FOGOW
    bool mStatic = false;

    bool mCanBeConq = false;

    bool mSelected = false;

    bool mVisible = true;
    bool mVisited = false;
    bool mLinked = false;
};

inline void GameObject::SetGameMap(GameMap * map) { mGameMap = map; }
inline void GameObject::SetScreen(ScreenGame * screen) { mScreen = screen; }

inline unsigned int GameObject::GetObjectId() const { return mObjId; }

inline IsoObject * GameObject::GetIsoObject() const { return mIsoObj; }

inline bool GameObject::IsStructure() const { return mStructure; }

inline bool GameObject::CanBeConquered() const { return mCanBeConq; }

inline bool GameObject::CanBeCollected() const { return GameObject::CAT_COLLECTABLE == mCategory; }

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

inline int GameObject::GetVisibilityLevel() const { return mVisLevel; }

inline PlayerFaction GameObject::GetFaction() const { return mFaction; }

inline GameObjectTypeId GameObject::GetObjectType() const { return mType; }

inline std::string GameObject::GetObjectTypeStr() const
{
    return GetObjectTypeStr(mType);
}

inline GameObjectCategoryId GameObject::GetObjectCategory() const { return mCategory; }

inline GameObjectVariantId GameObject::GetObjectVariant() const { return mVariant; }

inline unsigned int GameObject::GetRows() const { return mRows; }
inline unsigned int GameObject::GetCols() const { return mCols; }

inline GameMap * GameObject::GetGameMap() const { return mGameMap; }
inline ScreenGame * GameObject::GetScreen() const { return mScreen; }

inline void GameObject::SetVisibilityLevel(int val) { mVisLevel = val; }
inline void GameObject::SetStructure(bool val) { mStructure = val; }
inline void GameObject::SetCanBeConquered(bool val) { mCanBeConq = val; }
inline void GameObject::SetStatic(bool val) { mStatic = val; }

inline float GameObject::GetHealth() const { return mHealth; }
inline float GameObject::GetMaxHealth() const { return mMaxHealth; }
inline void GameObject::SetMaxHealth(float max) { mMaxHealth = max; }

inline float GameObject::GetEnergy() const { return mEnergy; }
inline float GameObject::GetMaxEnergy() const { return mMaxEnergy; }
inline void GameObject::SetMaxEnergy(float val) { mMaxEnergy = val; }

inline float GameObject::GetEnergyForActionStep(GameObjectActionType action) const
{
    if(action < NUM_OBJ_ACTIONS)
        return ACTION_COSTS[action];
    else
        return 0.f;
}

inline int GameObject::GetExperience() const { return mExp; }
inline int GameObject::GetExperienceToNextLevel() const { return mExpToNextLvl; }
inline int GameObject::GetExperienceLevel() const { return mExpLevel; }
inline int GameObject::GetMaxExperienceLevel() const { return 10; }

inline float GameObject::GetSpeed() const { return mSpeed; }
inline void GameObject::SetSpeed(float speed) { mSpeed = speed; }

inline GameObjectActionType GameObject::GetActiveAction() const { return mActiveAction; }
inline void GameObject::SetActiveAction(GameObjectActionType action) { mActiveAction = action; }
inline GameObjectActionType GameObject::GetCurrentAction() const { return mCurrAction; }
inline void GameObject::SetCurrentAction(GameObjectActionType action) { mCurrAction = action; }

} // namespace game
