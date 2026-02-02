#include "GameObject.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "GameMapCell.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObjectsGroup.h"
#include "GameObjects/ObjectInitData.h"
#include "GameObjectTools/Weapon.h"
#include "GameObjectTools/WeaponData.h"
#include "Particles/DataParticleDamage.h"
#include "Particles/DataParticleHitPoints.h"
#include "Particles/UpdaterDamage.h"
#include "Particles/UpdaterHitPoints.h"
#include "Screens/ScreenGame.h"
#include "Widgets/IconUpgrade.h"
#include "Widgets/WarningMessage.h"

#include <sgl/core/Math.h>
#include <sgl/graphic/Camera.h>
#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/UniformDistribution.h>

#include <cstdlib>

namespace
{
const float minDelta = 0.01f;

const float defMaxEnergy = 250.f;
const float defMaxHealth = 1000.f;
const float defMaxVisibility = 10.f;
const float defMaxVisibilityLinked = 20.f;
}

namespace game
{

const unsigned int GameObject::COLOR_FOW = 0x555555FF;
const unsigned int GameObject::COLOR_VIS = 0xFFFFFFFF;

unsigned int GameObject::counter = 0;

// -- OBJECT VARIANT --
const GameObjectVariantId GameObject::VAR_0 = 0;

const int GameObject::UPGRADE_POINTS[MAX_LEVEL] = { 1, 1, 2, 2, 2, 3, 3, 3, 3 };

// -- CONSTRUCTOR & DESTRUCTOR --
GameObject::GameObject(const ObjectData & data, const ObjectInitData & initData)
    : mAttributes(data.GetAttributes())
    , mIsoObj(new IsoObject(data.GetRows(), data.GetCols()))
    , mObjId(++counter)
    , mGame(initData.GetGame())
    , mGameMap(initData.GetGameMap())
    , mPartMan(initData.GetParticlesManager())
    , mOwner(initData.GetPlayer())
    , mType(data.GetType())
    , mCategory(data.GetCategory())
    , mRows(data.GetRows())
    , mCols(data.GetCols())
{
    // handle special case of Base which is always considered connected (to itself)
    // this avoids to call SetLinked to avoid virtual methods
    if(ObjectData::TYPE_BASE == mType)
        mLinked = true;

    // init faction based on owner
    if(mOwner != nullptr)
        mFaction = mOwner->GetFaction();
    else
        mFaction = NO_FACTION;

    // init colors for NO FACTION
    SetDefaultColors();

    // update data based on attributes
    SetMaxEnergy(defMaxEnergy);

    SetMaxHealth(defMaxHealth);

    GetRegenerationPower();

    SetMaxVisibilityLevel(defMaxVisibility, defMaxVisibilityLinked);
}

GameObject::~GameObject()
{
    delete mWarnMessage;

    delete mIsoObj;

    delete mWeapon;

    delete mIconUpgrade;

    ClearGroup();
}

// GROUP
void GameObject::SetGroup(GameObjectsGroup * g)
{
    // always clear current group
    ClearGroup();

    // but exit if g is NULL
    if(nullptr == g)
        return ;

    mGroup = g;
    mGroup->AddObject(this);
}

void GameObject::ClearGroup()
{
    if(nullptr == mGroup)
        return ;

    mGroup->RemoveObject(this);

    mGroup = nullptr;
}

void GameObject::SetOwner(Player * p)
{
    if(p == mOwner)
        return ;

    mOwner = p;

    mFaction = mOwner->GetFaction();

    OnFactionChanged();
}

void GameObject::OnPositionChanged() { }

int GameObject::GetX() const
{
    return mIsoObj->GetX();
}

int GameObject::GetY() const
{
    return mIsoObj->GetY();
}

void GameObject::SetPosition(int x, int y)
{
    mIsoObj->SetPosition(x, y);

    if(mIconUpgrade != nullptr)
        PositionIconUpgrade();

    if(mWarnMessage != nullptr && mWarnMessage->IsVisible())
    {
        mWarnMessage->FadeOut();
        PositionWarningMessage();
    }
}

void GameObject::SetX(int x)
{
    mIsoObj->SetX(x);

    if(mIconUpgrade != nullptr)
        PositionIconUpgrade();

    if(mWarnMessage != nullptr && mWarnMessage->IsVisible())
    {
        mWarnMessage->FadeOut();
        PositionWarningMessage();
    }
}
void GameObject::SetY(int y)
{
    mIsoObj->SetY(y);

    if(mIconUpgrade != nullptr)
        PositionIconUpgrade();

    if(mWarnMessage != nullptr && mWarnMessage->IsVisible())
    {
        mWarnMessage->FadeOut();
        PositionWarningMessage();
    }
}

void GameObject::ShowWarning(const char * text, float time)
{
    // show warning messages only for local player
    if(!IsFactionLocal())
        return ;

    // create the first time
    if(nullptr == mWarnMessage)
        mWarnMessage = new WarningMessage;

    mWarnMessage->ShowMessage(text, time);

    PositionWarningMessage();

    // play sound
    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("game/error_action_01.ogg");
}

void GameObject::SetSelected(bool val)
{
    // same value -> nothing to do
    if(val == mSelected)
        return ;

    mSelected = val;

    UpdateGraphics();
}

void GameObject::SetVisible(bool val)
{
    mVisible = val;

    UpdateGraphics();
}

void GameObject::SetLinked(bool val)
{
    if(val == mLinked)
        return ;

    mLinked = val;

    OnLinkedChanged();
}

void GameObject::SetCell(const GameMapCell * cell)
{
    mCell = cell;

    mIsoObj->SetRow(cell->row);
    mIsoObj->SetCol(cell->col);
}

int GameObject::GetVisibilityLevel() const
{
    return std::roundf(mMaxVisLevel * GetAttribute(OBJ_ATT_VIEW_RANGE) / MAX_STAT_FVAL);;
}

int GameObject::GetRow0() const { return mCell->row; }
int GameObject::GetCol0() const { return mCell->col; }
int GameObject::GetRow1() const { return 1 + mCell->row - mRows; }
int GameObject::GetCol1() const { return 1 + mCell->col - mCols; }

// NOTE this should only be used in the Map Editor
// for the game use SetOwner
bool GameObject::SetFaction(PlayerFaction f)
{
    // can't set the faction if an owner is already set
    if(mOwner != nullptr)
        return false;

    // nothing to do
    if(mFaction == f)
        return true;

    mFaction = f;

    OnFactionChanged();

    return true;
}

bool GameObject::IsFactionLocal() const
{
    if(mOwner != nullptr)
        return mOwner->IsLocal();
    else
        return false;
}

void GameObject::SetObjectVariant(GameObjectVariantId var)
{
    if(var == mVariant)
        return ;

    mVariant = var;

    UpdateGraphics();
}

bool GameObject::IsHealthMax() const
{
    const float maxH = GetMaxHealth();
    return  mHealth >= maxH || (maxH - mHealth) < minDelta;
}

float GameObject::GetMaxHealth() const
{
    return std::roundf(mMaxHealth * GetAttribute(OBJ_ATT_HEALTH) / MAX_STAT_FVAL);
}

void GameObject::SumHealth(float val)
{
    SetHealth(mHealth + val);
}

bool GameObject::IsEnergyMax() const
{
    const float maxEn = GetMaxEnergy();
    return  mEnergy >= maxEn  || (maxEn - mEnergy) < minDelta;
}

void GameObject::SumEnergy(float val)
{
    SetEnergy(mEnergy + val);
}

float GameObject::GetMaxEnergy() const
{
    return std::roundf(mMaxEnergy * GetAttribute(OBJ_ATT_ENERGY) / MAX_STAT_FVAL);
}

bool GameObject::HasEnergyForActionStep(GameObjectActionType action) const
{
    if(action < NUM_OBJ_ACTIONS)
    {
        const float cost = (ATTACK == action) ? mWeapon->GetCostEnergy() :
                           GetActionEnergyCost(action);

        // turn energy is only valid for units
        if(mOwner != nullptr && mCategory == ObjectData::CAT_UNIT)
            return GetEnergy() >= cost && mOwner->GetTurnEnergy() >= cost;
        else
            return GetEnergy() >= cost;
    }
    else
        return false;
}

void GameObject::ActionStepCompleted(GameObjectActionType action)
{
    if(action >= NUM_OBJ_ACTIONS)
        return ;

    // ENERGY
    const float costEnergy = -GetActionEnergyCost(action);

    SumEnergy(costEnergy);

    // turn energy is only valid for units
    if(mOwner != nullptr && mCategory == ObjectData::CAT_UNIT)
        mOwner->SumTurnEnergy(costEnergy);

    // EXPERIENCE
    SumExperience(GetActionExperienceGain(action));
}

int GameObject::GetExperienceToLevel(int level) const
{
    // TODO use different values according to difficulty level
    // increments
    //  0, 50, 100, 150, 250, 400, 650, 1050, 1700, 2750
    //  0, 50, 150, 200, 350, 550, 900, 1450, 2350, 3800
    //  0, 50, 200, 250, 450, 700, 1150, 1850, 3000, 4850
    const int points[MAX_LEVEL] =
    {
      //0, 50, 150, 300, 550, 950, 1600, 2650, 4350, 7100
        0, 50, 200, 400, 750, 1300, 2200, 3650, 6000, 9800
      //0, 50, 250, 500, 950, 1650, 2800, 4650, 7650, 12500
    };

    if(level < MAX_LEVEL)
        return points[level];
    else
        return points[MAX_LEVEL - 1];
}

void GameObject::SumExperience(int val)
{
    SetExperience(val + mExp);
}

void GameObject::UpgradeLevel(const std::vector<int> & attChanges)
{
    // can't upgrade yet or already at max level -> exit
    if(mExp < GetExperienceToNextLevel() || mExpLevel == MAX_LEVEL)
        return;

    // increase level
    ++mExpLevel;

    // hide icon upgrade if exp is below new next level or reached max
    if(mExp < GetExperienceToNextLevel() || mExpLevel == MAX_LEVEL)
        HideIconUpgrade();

    // update attributes
    const unsigned int numAtt = attChanges.size();

    for(unsigned int i = 0; i < numAtt; ++i)
        mAttributes[static_cast<ObjAttId>(i)] += attChanges[i];

    // update visibility if needed
    if(IsFactionLocal() && attChanges[OBJ_ATT_VIEW_RANGE] > 0)
        mGameMap->UpdateLocalObjectVisibility(this);

    // notify object and observers
    OnAttributeChanged();

    NotifyValueChanged();
}

unsigned int GameObject::AddFunctionOnValueChanged(const std::function<void()> & f)
{
    static unsigned int num = 0;

    int fId = ++num;
    mOnValueChanged.emplace(fId, f);

    return fId;
}

void GameObject::RemoveFunctionOnValueChanged(unsigned int fId)
{
    auto it = mOnValueChanged.find(fId);

    if(it != mOnValueChanged.end())
        mOnValueChanged.erase(it);
}

int GameObject::GetAttribute(ObjAttId attID) const
{
    const auto it = mAttributes.find(attID);

    return (it != mAttributes.end()) ? it->second : 0;
}

float GameObject::GetSpeed() const
{
#ifdef DEV_MODE
    if(Game::GOD_MODE && IsFactionLocal())
        return 10.f;
#endif

    return mMaxSpeed * GetAttribute(OBJ_ATT_SPEED) / MAX_STAT_FVAL;
}

void GameObject::SetWeapon(Weapon * w)
{
    mWeapon = w;

    // copy attributes from weapon to object
    const std::unordered_map<ObjAttId, int> & wAtt = w->GetAttributes();

    for(auto wIt = wAtt.begin(); wIt != wAtt.end(); ++wIt)
    {
        auto objIt = mAttributes.find(wIt->first);

        // attribute alredy set -> update it
        if(objIt != mAttributes.end())
            objIt->second = wIt->second;
        // attribute not set yet -> create it
        else
            mAttributes.emplace(wIt->first, wIt->second);
    }
}

void GameObject::SetAttackMode(AttackMode am)
{
    if(mWeapon != nullptr)
        mWeapon->SetAttackMode(am);
}

void GameObject::FindAndSetEnemyTarget()
{
    if(mWeapon == nullptr)
        return ;

    auto gm = GetGameMap();

    const std::vector<GameMapCell> & cells = gm->GetCells();
    const int mapRows = gm->GetNumRows();
    const int mapCols = gm->GetNumCols();

    const int cr = GetRow0();
    const int cc = GetCol0();

    const int rad = mWeapon->GetRange();
    const int r0 = cr >= rad ? cr - rad : 0;
    const int r1 = cr + rad < mapRows ? cr + rad + 1 : mapRows;
    const int c0 = cc >= rad ? cc - rad : 0;
    const int c1 = cc + rad < mapCols ? cc + rad + 1 : mapCols;

    GameObject * target = nullptr;
    int minDist = mapRows + mapCols;

    for(int r = r0; r < r1; ++r)
    {
        const int ind0 = r * mapCols;

        for(int c = c0; c < c1; ++c)
        {
            const int ind = ind0 + c;
            const GameMapCell & cell = cells[ind];

            // prioritize object on top
            GameObject * obj = cell.objTop != nullptr ? cell.objTop : cell.objBottom;

            // enemy found
            if(obj != nullptr && obj->GetFaction() != mFaction && obj->GetFaction() != NO_FACTION)
            {
                const int dist = std::abs(cr - r) + std::abs(cc - c);

                // enemy is closer than others
                if(dist < minDist)
                {
                    minDist = dist;
                    target = cell.objTop != nullptr ? cell.objTop : cell.objBottom;
                }
            }
        }
    }

    if(target != nullptr)
        mWeapon->SetTarget(target);
    else
        mWeapon->ClearTarget();
}

bool GameObject::HasEnemyInRange()
{
    if(mWeapon == nullptr)
        return false;

    auto gm = GetGameMap();

    const std::vector<GameMapCell> & cells = gm->GetCells();
    const int mapRows = gm->GetNumRows();
    const int mapCols = gm->GetNumCols();

    const int cr = GetRow0();
    const int cc = GetCol0();

    const int rad = mWeapon->GetRange();
    const int r0 = cr >= rad ? cr - rad : 0;
    const int r1 = cr + rad < mapRows ? cr + rad + 1 : mapRows;
    const int c0 = cc >= rad ? cc - rad : 0;
    const int c1 = cc + rad < mapCols ? cc + rad + 1 : mapCols;

    for(int r = r0; r < r1; ++r)
    {
        const int ind0 = r * mapCols;

        for(int c = c0; c < c1; ++c)
        {
            const int ind = ind0 + c;
            const GameMapCell & cell = cells[ind];

            // prioritize object on top
            GameObject * obj = cell.objTop != nullptr ? cell.objTop : cell.objBottom;

            // enemy found
            if(obj != nullptr && obj->GetFaction() != mFaction && obj->GetFaction() != NO_FACTION)
                return true;
        }
    }

    return false;
}

void GameObject::Hit(float damage, GameObject * attacker, bool fatal, bool showHitPoints)
{
    using namespace sgl;

    // already destroyed -> do nothing
    if(IsDestroyed())
        return ;

    // fatal hit
    if(fatal)
        damage = GetMaxHealth();
    // standard hit
    else
    {
        // damage is influnced by object's resistance
        const float fixedW = 0.5f;
        const float variableW = 1.f - fixedW;
        const float variableDamage = 1.f - (GetAttribute(OBJ_ATT_RESISTANCE) / MAX_STAT_FVAL);

        damage = damage * fixedW + (damage * variableW * variableDamage);

        // shield can block up to shieldEfficency% of hit
        const float shieldEfficency = 0.15f;
        damage -= damage * shieldEfficency * (GetAttribute(OBJ_ATT_SHIELD) / MAX_STAT_FVAL);
    }

    damage = std::roundf(damage);

    SumHealth(-damage);

    const int numPart0 = 30 * mRows * mCols;
    int numPart = numPart0;

    const int maxQuad = 4;
    int numQuad = 1;

    const float angInc = 90.f;
    float ang0 = 0.f;

    // hit, but alive -> pick random quadrant
    if(mHealth > 0.f)
    {
        const int quad0 = 0;
        utilities::UniformDistribution genQuad(quad0, maxQuad - 1, mGame->GetRandSeed());

        ang0 += angInc * genQuad.GetNextValue();

        const int partDestroyedMult = 2;
        numPart *= partDestroyedMult;
    }
    // hit and destroyed -> use all quadrants
    else
    {
        const int multPart = maxQuad;
        numPart *= multPart;

        numQuad = maxQuad;

        // record stats for players
        // NOTE register kills only when destroying enemies
        if(mOwner != nullptr)
        {
            if(attacker != nullptr)
                mGameMap->RegisterEnemyKill(attacker);

            mGameMap->RegisterCasualty(GetFaction());
        }
    }

    float ang1 = ang0 + angInc;

    const int numPartQuad = numPart / numQuad;

    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterDamage *>(partMan->GetUpdater(PU_DAMAGE));

    const unsigned int texInd = SpriteIdParticles::ID_PART_RECT_4x4;
    auto tex = graphic::TextureManager::Instance()->GetSprite(SpriteFileParticles, texInd);

    IsoObject * isoObj = GetIsoObject();
    const float objXC = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float objYC = isoObj->GetY() + isoObj->GetHeight() * 0.5f;

    // random generator of rotation angle
    const int minRot = 0;
    const int maxRot = 360;
    utilities::UniformDistribution genRot(minRot, maxRot, mGame->GetRandSeed());

    // random generator for velocity direction
    utilities::UniformDistribution genVel(static_cast<int>(ang0), static_cast<int>(ang1),
                                          mGame->GetRandSeed());

    const float deg2rad = sgl::core::Math::PIf / 180.f;

    // random generator for speed
    const int minSpeed = 100;
    const int maxSpeed = 300;
    utilities::UniformDistribution genSpeed(minSpeed, maxSpeed, mGame->GetRandSeed());

    // random generator for decay speed
    const int minDecSpeed = 200;
    const int maxDecSpeed = 400;
    utilities::UniformDistribution genDecSpeed(minDecSpeed, maxDecSpeed, mGame->GetRandSeed());

    // random generator for scale
    const int minScale = 1;
    const int maxScale = 2;
    utilities::UniformDistribution genScale(minScale, maxScale, mGame->GetRandSeed());

    // random generator for color
    const int color0 = 0;
    const int colorN = mObjColors.size() - 1;

    utilities::UniformDistribution genColor(color0, colorN, mGame->GetRandSeed());

    for(int q = 0; q < numQuad; ++q)
    {
        for(int p = 0; p < numPartQuad; ++p)
        {
            const double rot = genRot.GetNextValue();

            const float ang = genVel.GetNextValue() * deg2rad;
            const float velX = cosf(ang);
            const float velY = sinf(ang);

            const float speed = genSpeed.GetNextValue();

            const float decSpeed = genDecSpeed.GetNextValue();

            const unsigned int color = mObjColors[genColor.GetNextValue()];

            const float scale = 1.f / static_cast<float>(genScale.GetNextValue());

            DataParticleDamage data(tex, rot, objXC, objYC, velX, velY, speed, decSpeed, scale, color);

            pu->AddParticle(data);
        }

        ang0 += angInc;
        ang1 += angInc;

        genVel.SetParameters(static_cast<int>(ang0), static_cast<int>(ang1));
    }

    // -- HIT POINTS --
    if(!showHitPoints)
        return ;

    // random generator for X position
    const int maxXDeltaHP = isoObj->GetWidth() * 0.25;
    const int minXDeltaHP = -maxXDeltaHP;

    utilities::UniformDistribution genPosHP(minXDeltaHP, maxXDeltaHP, mGame->GetRandSeed());

    const float posXHP = objXC + genPosHP.GetNextValue();
    const float posYHP = objYC - (isoObj->GetHeight() * 0.25f);

    const float speedHP = 75.f;
    const float decaySpeedHP = 50.f;
    const float maxDistHP = 50.f;

    auto puHP = static_cast<UpdaterHitPoints *>(partMan->GetUpdater(PU_HIT_POINTS));

    DataParticleHitPoints dataHP(damage, posXHP, posYHP, speedHP, decaySpeedHP, maxDistHP, fatal);
    puHP->AddParticle(dataHP);
}

void GameObject::MissHit()
{
    auto partMan = GetParticlesManager();
    auto pu = static_cast<UpdaterDamage *>(partMan->GetUpdater(PU_DAMAGE));

    IsoObject * isoObj = GetIsoObject();
    const float posX = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float posY = isoObj->GetY();

    const float speedHP = 75.f;
    const float decaySpeedHP = 50.f;
    const float maxDistHP = 50.f;

    auto puHP = static_cast<UpdaterHitPoints *>(partMan->GetUpdater(PU_HIT_POINTS));

    DataParticleHitPoints dataHP(posX, posY, speedHP, decaySpeedHP, maxDistHP);
    puHP->AddParticle(dataHP);
}

void GameObject::SelfDestroy() { Hit(0.f, nullptr, true, false); }

void GameObject::SetActiveActionToDefault() { mActiveAction = mDefaultAction; }

void GameObject::OnNewTurn(PlayerFaction faction)
{
    if(GetFaction() == faction)
        RestoreTurnEnergy();
}

void GameObject::Update(float) { }

void GameObject::OnFactionChanged()
{
    SetDefaultColors();
    UpdateGraphics();
}

void GameObject::OnLinkedChanged()
{
    SetMaxVisibilityLevel(defMaxVisibility, defMaxVisibilityLinked);
}

void GameObject::OnAttributeChanged()
{
}

void GameObject::NotifyValueChanged()
{
    for(const auto & it : mOnValueChanged)
        it.second();
}

float GameObject::GetTime(float maxTime, float attribute) const
{
#ifdef DEV_MODE
    if(Game::GOD_MODE)
        return TIME_GOD_MODE;
#endif

    // special time for invisible AI
    if(mOwner != nullptr && !mOwner->IsLocal() && !GetGameMap()->IsObjectVisibleToLocalPlayer(this))
        return TIME_AI_MIN;

    const float baseTime = 0.25f;

    return baseTime + maxTime - (maxTime * attribute / MAX_STAT_FVAL);
}

void GameObject::SetMaxVisibilityLevel(float maxVal, float maxValLinked)
{
    mMaxVisLevel = IsLinked() ? maxValLinked : maxVal;
}

float GameObject::GetRegenerationPower() const
{
    return GetAttribute(OBJ_ATT_REGENERATION) / MAX_STAT_FVAL;
}

float GameObject::GetActionEnergyCost(GameObjectActionType action) const
{
    if(ATTACK == action)
        return mWeapon->GetCostEnergyPerShot();

    constexpr float ACTION_COSTS[NUM_OBJ_ACTIONS] =
    {
        0.f,        // IDLE
        10.f,       // BUILD_UNIT
        0.f,        // SET_TARGET
        5.f,        // MOVE
        10.f,       // CONQUER_CELL
        20.f,       // CONQUER_STRUCTURE
        0.f,        // ATTACK
        30.f,       // BUILD_STRUCTURE
        10.f,       // BUILD_WALL
        5.f,        // HEAL
        4.f,        // SPAWN
        1.f,        // TOGGLE_GATE
        0.f,        // SELF_DESTRUCTION
    };

    return ACTION_COSTS[action];
}

float GameObject::GetActionExperienceGain(GameObjectActionType action) const
{
    constexpr int ACTION_EXPERIENCE[NUM_OBJ_ACTIONS] =
    {
        0,      // IDLE
        10,     // BUILD_UNIT
        0,      // SET_TARGET
        1,      // MOVE
        2,      // CONQUER_CELL
        5,      // CONQUER_STRUCTURE
        1,      // ATTACK
        5,      // BUILD_STRUCTURE
        2,      // BUILD_WALL
        2,      // HEAL
        5,      // SPAWN
        1,      // TOGGLE_GATE
        0,      // SELF_DESTRUCTION
    };

    return ACTION_EXPERIENCE[action];
}

void GameObject::PositionWarningMessage()
{
    const auto camera = sgl::graphic::Camera::GetDefaultCamera();

    const int objX = mIsoObj->GetX();
    const int objY = mIsoObj->GetY();
    const int marginB = 5;

    int messX = objX + (mIsoObj->GetWidth() - mWarnMessage->GetWidth()) / 2;
    int messY = objY - mWarnMessage->GetHeight() - marginB;

    if(camera->GetWorldToScreenX(messX) < 0)
        messX = camera->GetX();

    // move below object if can't see warning above it
    if(camera->GetWorldToScreenY(messY) < 0)
        messY = objY + mIsoObj->GetHeight() + marginB;

    mWarnMessage->SetPosition(messX, messY);
}

void GameObject::ShowIconUpgrade()
{
    // already showing it
    if(mIconUpgrade != nullptr)
        return ;

    // only show for local player
    if(!IsFactionLocal())
        return;

    mIconUpgrade = new IconUpgrade(mFaction);

    // play sound
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("game/upgrade_notification-01.ogg");

    PositionIconUpgrade();
}

void GameObject::HideIconUpgrade()
{
    delete mIconUpgrade;
    mIconUpgrade = nullptr;
}

void GameObject::PositionIconUpgrade()
{
    const int isoX = mIsoObj->GetX();
    const int isoY = mIsoObj->GetY();
    const int isoW = mIsoObj->GetWidth();

    const int iconMarginV = 5;
    const int iconX = isoX + (isoW - mIconUpgrade->GetWidth()) / 2;
    const int iconY = isoY - mIconUpgrade->GetHeight() - iconMarginV;

    mIconUpgrade->SetPosition(iconX, iconY);
}

void GameObject::SetEnergy(float val)
{
    const float oldEn = mEnergy;

    mEnergy = val;

    const float maxEn = GetMaxEnergy();

    if(mEnergy > maxEn || (maxEn - mEnergy) < minDelta)
        mEnergy = maxEn;
    else if(mEnergy < 0.f)
        mEnergy = 0.f;

#ifdef DEV_MODE
    if(Game::GOD_MODE && IsFactionLocal())
        mEnergy = val;
#endif

    const float diff = std::fabs(mEnergy - oldEn);

    if(diff > minDelta)
        NotifyValueChanged();
}

void GameObject::SetExperience(int val)
{
    if(val == mExp)
        return ;

    mExp = val;

    if(mExp >= GetExperienceToNextLevel())
        ShowIconUpgrade();
    else
        HideIconUpgrade();

    NotifyValueChanged();
}

void GameObject::SetHealth(float val)
{
    const float oldH = mHealth;

    mHealth = val;

    const float maxH = GetMaxHealth();

    if(mHealth > maxH || (maxH - mHealth) < minDelta)
        mHealth = maxH;
    else if(mHealth < 0.f)
        mHealth = 0.f;

    const float diff = std::fabs(mHealth - oldH);

    if(diff > minDelta)
        NotifyValueChanged();
}

void GameObject::RestoreTurnEnergy()
{
    const float basePerc = 0.5f;
    const float maxEn = GetMaxEnergy();
    const float regPower = GetRegenerationPower();
    const float baseEnergy = maxEn * basePerc;
    const float newEnergy = (maxEn - baseEnergy) * regPower;
    const float prevEnergy = mEnergy * regPower;
    const float energy = std::roundf(baseEnergy + prevEnergy + newEnergy);

    SumEnergy(energy);

#ifdef DEV_MODE
    if(Game::GOD_MODE && IsFactionLocal())
        SetEnergy(mMaxEnergy * 5);
#endif
}

void GameObject::SetDefaultColors()
{
    // clear current colors
    mObjColors.clear();

    // assign new colors based on faction
    switch(GetFaction())
    {
        case FACTION_1:
            mObjColors.push_back(0xd9938cff);
            mObjColors.push_back(0xcc6f66ff);
            mObjColors.push_back(0xc04a3fff);
            mObjColors.push_back(0xcc4133ff);
            mObjColors.push_back(0x9a3b32ff);
            mObjColors.push_back(0x86332cff);
        break;

        case FACTION_2:
            mObjColors.push_back(0x8cd992ff);
            mObjColors.push_back(0x66cc6eff);
            mObjColors.push_back(0x3fc04aff);
            mObjColors.push_back(0x33cc40ff);
            mObjColors.push_back(0x329a3bff);
            mObjColors.push_back(0x2c8633ff);
        break;

        case FACTION_3:
            mObjColors.push_back(0x8cccd9ff);
            mObjColors.push_back(0x66bbccff);
            mObjColors.push_back(0x3faac0ff);
            mObjColors.push_back(0x33b2ccff);
            mObjColors.push_back(0x32899aff);
            mObjColors.push_back(0x2c7786ff);
        break;

        // NO_FACTION
        default:
            mObjColors.push_back(0xccccccff);
            mObjColors.push_back(0xb2b2b2ff);
            mObjColors.push_back(0x999999ff);
            mObjColors.push_back(0x808080ff);
            mObjColors.push_back(0x666666ff);
            mObjColors.push_back(0x595959ff);
        break;
    }
}

} // namespace game
