#include "GameObject.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "GameMapCell.h"
#include "IsoObject.h"
#include "Player.h"
#include "Particles/DataParticleDamage.h"
#include "Particles/UpdaterDamage.h"
#include "Screens/ScreenGame.h"

#include <sgl/core/Math.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/UniformDistribution.h>

namespace game
{

using h = std::hash<std::string>;

const unsigned int GameObject::COLOR_FOW = 0x555555FF;
const unsigned int GameObject::COLOR_VIS = 0xFFFFFFFF;

unsigned int GameObject::counter = 0;

const float GameObject::ACTION_COSTS[NUM_OBJ_ACTIONS] =
{
    0.f,        // IDLE
    10.f,       // BUILD_UNIT
    5.f,        // MOVE
    10.f,       // CONQUER_CELL
    20.f,       // CONQUER_STRUCTURE
    2.f,        // ATTACK
    30.f,       // BUILD_STRUCTURE
    10.f,       // BUILD_WALL
    5.f,        // HEAL
    1.f         // TOGGLE_GATE
};

const int GameObject::ACTION_EXPERIENCE[NUM_OBJ_ACTIONS] =
{
    0,      // IDLE
    10,     // BUILD_UNIT
    1,      // MOVE
    2,      // CONQUER_CELL
    5,      // CONQUER_STRUCTURE
    1,      // ATTACK
    5,      // BUILD_STRUCTURE
    2,      // BUILD_WALL
    2,      // HEAL
    1,      // TOGGLE_GATE
};

// -- OBJECT TYPE --
const std::string GameObject::TYPE_STR_BARRACKS("BARRACKS");
const std::string GameObject::TYPE_STR_BASE("BASE");
const std::string GameObject::TYPE_STR_BASE_SPOT("BASE_SPOT");
const std::string GameObject::TYPE_STR_BLOBS("BLOBS");
const std::string GameObject::TYPE_STR_DEFENSIVE_TOWER("DEF_TOWER");
const std::string GameObject::TYPE_STR_DIAMONDS("DIAMONDS");
const std::string GameObject::TYPE_STR_HOSPITAL("HOSPITAL");
const std::string GameObject::TYPE_STR_LOOTBOX("LOOTBOX");
const std::string GameObject::TYPE_STR_MOUNTAINS("MOUNTAINS");
const std::string GameObject::TYPE_STR_PRACTICE_TARGET("TARGET");
const std::string GameObject::TYPE_STR_RADAR_STATION("RADAR_STATION");
const std::string GameObject::TYPE_STR_RADAR_TOWER("RADAR_TOWER");
const std::string GameObject::TYPE_STR_RESEARCH_CENTER("RESEARCH_CENTER");
const std::string GameObject::TYPE_STR_RES_GEN_ENERGY("RESGEN_ENER");
const std::string GameObject::TYPE_STR_RES_GEN_ENERGY_SOLAR("RESGEN_SOLAR");
const std::string GameObject::TYPE_STR_RES_GEN_MATERIAL("RESGEN_MAT");
const std::string GameObject::TYPE_STR_RES_GEN_MATERIAL_EXTRACT("RESGEN_MAT_EXT");
const std::string GameObject::TYPE_STR_RES_STORAGE_BLOBS("RESSTOR_BLOBS");
const std::string GameObject::TYPE_STR_RES_STORAGE_DIAMONDS("RESSTOR_DIAM");
const std::string GameObject::TYPE_STR_RES_STORAGE_ENERGY("RESSTOR_ENER");
const std::string GameObject::TYPE_STR_RES_STORAGE_MATERIAL("RESSTOR_MAT");
const std::string GameObject::TYPE_STR_ROCKS("ROCKS");
const std::string GameObject::TYPE_STR_TEMPLE("TEMPLE");
const std::string GameObject::TYPE_STR_TREES("TREES");
const std::string GameObject::TYPE_STR_UNIT_MEDIC1("UNIT_MEDIC1");
const std::string GameObject::TYPE_STR_UNIT_SCOUT1("UNIT_SCOUT1");
const std::string GameObject::TYPE_STR_UNIT_SOLDIER1("UNIT_SOLDIER1");
const std::string GameObject::TYPE_STR_UNIT_SOLDIER2("UNIT_SOLDIER2");
const std::string GameObject::TYPE_STR_UNIT_WORKER1("UNIT_WORKER1");
const std::string GameObject::TYPE_STR_WALL("WALL");
const std::string GameObject::TYPE_STR_WALL_GATE("WALL_GATE");

const GameObjectTypeId GameObject::TYPE_NULL = 0;

const GameObjectTypeId GameObject::TYPE_BARRACKS = h{}(TYPE_STR_BARRACKS);
const GameObjectTypeId GameObject::TYPE_BASE = h{}(TYPE_STR_BASE);
const GameObjectTypeId GameObject::TYPE_BASE_SPOT = h{}(TYPE_STR_BASE_SPOT);
const GameObjectTypeId GameObject::TYPE_BLOBS = h{}(TYPE_STR_BLOBS);
const GameObjectTypeId GameObject::TYPE_DEFENSIVE_TOWER = h{}(TYPE_STR_DEFENSIVE_TOWER);
const GameObjectTypeId GameObject::TYPE_DIAMONDS = h{}(TYPE_STR_DIAMONDS);
const GameObjectTypeId GameObject::TYPE_HOSPITAL = h{}(TYPE_STR_HOSPITAL);
const GameObjectTypeId GameObject::TYPE_LOOTBOX = h{}(TYPE_STR_LOOTBOX);
const GameObjectTypeId GameObject::TYPE_MOUNTAINS = h{}(TYPE_STR_MOUNTAINS);
const GameObjectTypeId GameObject::TYPE_PRACTICE_TARGET = h{}(TYPE_STR_PRACTICE_TARGET);
const GameObjectTypeId GameObject::TYPE_RADAR_STATION = h{}(TYPE_STR_RADAR_STATION);
const GameObjectTypeId GameObject::TYPE_RADAR_TOWER = h{}(TYPE_STR_RADAR_TOWER);
const GameObjectTypeId GameObject::TYPE_RESEARCH_CENTER = h{}(TYPE_STR_RESEARCH_CENTER);
const GameObjectTypeId GameObject::TYPE_RES_GEN_ENERGY = h{}(TYPE_STR_RES_GEN_ENERGY);
const GameObjectTypeId GameObject::TYPE_RES_GEN_ENERGY_SOLAR = h{}(TYPE_STR_RES_GEN_ENERGY_SOLAR);
const GameObjectTypeId GameObject::TYPE_RES_GEN_MATERIAL = h{}(TYPE_STR_RES_GEN_MATERIAL);
const GameObjectTypeId GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT = h{}(TYPE_STR_RES_GEN_MATERIAL_EXTRACT);
const GameObjectTypeId GameObject::TYPE_RES_STORAGE_BLOBS = h{}(TYPE_STR_RES_STORAGE_BLOBS);
const GameObjectTypeId GameObject::TYPE_RES_STORAGE_DIAMONDS = h{}(TYPE_STR_RES_STORAGE_DIAMONDS);
const GameObjectTypeId GameObject::TYPE_RES_STORAGE_ENERGY = h{}(TYPE_STR_RES_STORAGE_ENERGY);
const GameObjectTypeId GameObject::TYPE_RES_STORAGE_MATERIAL = h{}(TYPE_STR_RES_STORAGE_MATERIAL);
const GameObjectTypeId GameObject::TYPE_ROCKS = h{}(TYPE_STR_ROCKS);
const GameObjectTypeId GameObject::TYPE_TEMPLE = h{}(TYPE_STR_TEMPLE);
const GameObjectTypeId GameObject::TYPE_TREES = h{}(TYPE_STR_TREES);
const GameObjectTypeId GameObject::TYPE_UNIT_MEDIC1 = h{}(TYPE_STR_UNIT_MEDIC1);
const GameObjectTypeId GameObject::TYPE_UNIT_SCOUT1 = h{}(TYPE_STR_UNIT_SCOUT1);
const GameObjectTypeId GameObject::TYPE_UNIT_SOLDIER1 = h{}(TYPE_STR_UNIT_SOLDIER1);
const GameObjectTypeId GameObject::TYPE_UNIT_SOLDIER2 = h{}(TYPE_STR_UNIT_SOLDIER2);
const GameObjectTypeId GameObject::TYPE_UNIT_WORKER1 = h{}(TYPE_STR_UNIT_WORKER1);
const GameObjectTypeId GameObject::TYPE_WALL = h{}(TYPE_STR_WALL);
const GameObjectTypeId GameObject::TYPE_WALL_GATE = h{}(TYPE_STR_WALL_GATE);

const std::unordered_map<GameObjectTypeId, std::string> GameObject::TYPE_STR_MAP =
{
    { GameObject::TYPE_BARRACKS, TYPE_STR_BARRACKS},
    { GameObject::TYPE_BASE, TYPE_STR_BASE},
    { GameObject::TYPE_BASE_SPOT, TYPE_STR_BASE_SPOT},
    { GameObject::TYPE_DEFENSIVE_TOWER, TYPE_STR_DEFENSIVE_TOWER },
    { GameObject::TYPE_HOSPITAL, TYPE_STR_HOSPITAL },
    { GameObject::TYPE_LOOTBOX, TYPE_STR_LOOTBOX },
    { GameObject::TYPE_MOUNTAINS, TYPE_STR_MOUNTAINS },
    { GameObject::TYPE_PRACTICE_TARGET, TYPE_STR_PRACTICE_TARGET },
    { GameObject::TYPE_RADAR_STATION, TYPE_STR_RADAR_STATION },
    { GameObject::TYPE_RADAR_TOWER, TYPE_STR_RADAR_TOWER },
    { GameObject::TYPE_RESEARCH_CENTER, TYPE_STR_RESEARCH_CENTER },
    { GameObject::TYPE_RES_GEN_ENERGY, TYPE_STR_RES_GEN_ENERGY },
    { GameObject::TYPE_RES_GEN_ENERGY_SOLAR, TYPE_STR_RES_GEN_ENERGY_SOLAR },
    { GameObject::TYPE_RES_GEN_MATERIAL, TYPE_STR_RES_GEN_MATERIAL },
    { GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT, TYPE_STR_RES_GEN_MATERIAL_EXTRACT },
    { GameObject::TYPE_RES_STORAGE_BLOBS, TYPE_STR_RES_STORAGE_BLOBS },
    { GameObject::TYPE_RES_STORAGE_DIAMONDS, TYPE_STR_RES_STORAGE_DIAMONDS },
    { GameObject::TYPE_RES_STORAGE_ENERGY, TYPE_STR_RES_STORAGE_ENERGY },
    { GameObject::TYPE_RES_STORAGE_MATERIAL, TYPE_STR_RES_STORAGE_MATERIAL },
    { GameObject::TYPE_ROCKS, TYPE_STR_ROCKS },
    { GameObject::TYPE_TEMPLE, TYPE_STR_TEMPLE },
    { GameObject::TYPE_TREES, TYPE_STR_TREES },
    { GameObject::TYPE_UNIT_MEDIC1, TYPE_STR_UNIT_MEDIC1 },
    { GameObject::TYPE_UNIT_SCOUT1, TYPE_STR_UNIT_SCOUT1 },
    { GameObject::TYPE_UNIT_SOLDIER1, TYPE_STR_UNIT_SOLDIER1 },
    { GameObject::TYPE_UNIT_SOLDIER2, TYPE_STR_UNIT_SOLDIER2 },
    { GameObject::TYPE_UNIT_WORKER1, TYPE_STR_UNIT_WORKER1 },
    { GameObject::TYPE_WALL, TYPE_STR_WALL },
    { GameObject::TYPE_WALL_GATE, TYPE_STR_WALL_GATE }
};

std::string GameObject::GetObjectTypeStr(const GameObjectTypeId type)
{
    static const std::string noStr;

    auto it = TYPE_STR_MAP.find(type);

    if(it != TYPE_STR_MAP.end())
        return it->second;
    else
        return noStr;
}

const std::unordered_map<GameObjectTypeId, std::string> GameObject::TITLES =
{
    { GameObject::TYPE_BARRACKS, "BARRACKS"},
    { GameObject::TYPE_BASE, "BASE"},
    { GameObject::TYPE_BASE_SPOT, "BASE SPOT"},
    { GameObject::TYPE_DEFENSIVE_TOWER, "DEFENSIVE TOWER"},
    { GameObject::TYPE_HOSPITAL, "HOSPITAL"},
    { GameObject::TYPE_LOOTBOX, "LOOT BOX"},
    { GameObject::TYPE_MOUNTAINS, "MOUNTAINS"},
    { GameObject::TYPE_PRACTICE_TARGET, "PRACTICE TARGET"},
    { GameObject::TYPE_RADAR_STATION, "RADAR STATION"},
    { GameObject::TYPE_RADAR_TOWER, "RADAR TOWER"},
    { GameObject::TYPE_RESEARCH_CENTER, "RESEARCH CENTER"},
    { GameObject::TYPE_RES_GEN_ENERGY, "ENERGY GENERATOR"},
    { GameObject::TYPE_RES_GEN_ENERGY_SOLAR, "SOLAR PANELS"},
    { GameObject::TYPE_RES_GEN_MATERIAL, "MATERIAL GENERATOR"},
    { GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT, "MATERIAL EXTRACTOR"},
    { GameObject::TYPE_RES_STORAGE_BLOBS, "BLOBS STORAGE"},
    { GameObject::TYPE_RES_STORAGE_DIAMONDS, "DIAMONDS STORAGE"},
    { GameObject::TYPE_RES_STORAGE_ENERGY, "ENERGY STORAGE"},
    { GameObject::TYPE_RES_STORAGE_MATERIAL, "MATERIAL STORAGE"},
    { GameObject::TYPE_ROCKS, "ROCKS"},
    { GameObject::TYPE_TEMPLE, "TEMPLE"},
    { GameObject::TYPE_TREES, "TREES"},
    { GameObject::TYPE_UNIT_MEDIC1, "MEDIC"},
    { GameObject::TYPE_UNIT_SCOUT1, "SCOUT"},
    { GameObject::TYPE_UNIT_SOLDIER1, "SOLDIER"},
    { GameObject::TYPE_UNIT_SOLDIER2, "SOLDIER"},
    { GameObject::TYPE_UNIT_WORKER1, "WORKER"},
    { GameObject::TYPE_WALL, "WALL"},
    { GameObject::TYPE_WALL_GATE, "GATE"}
};

const std::unordered_map<GameObjectTypeId, std::string> GameObject::DESCRIPTIONS =
{
    { GameObject::TYPE_BARRACKS, "A structure that creates soldier units."},
    { GameObject::TYPE_BASE, "A control center. It can create worker units."
                             "You need to defend it if you don't want to lose a territory."},
    { GameObject::TYPE_BASE_SPOT, "This represents where a faction base will be placed. "
                                  "Numbers identify the factions in a map."},
    { GameObject::TYPE_DEFENSIVE_TOWER, "A basic defensive tower."},
    { GameObject::TYPE_HOSPITAL, "A structure that creates medics and that can heal units."},
    { GameObject::TYPE_LOOTBOX, "A loot box that can be collected by a unit to obtain various resources."},
    { GameObject::TYPE_MOUNTAINS, "Some mountains"},
    { GameObject::TYPE_PRACTICE_TARGET, "A practice target.\nIt can be used to train your units "
                                        "and to improve their attack skills."},
    { GameObject::TYPE_RADAR_STATION, "A powerful radar that can show a big portion of the map."},
    { GameObject::TYPE_RADAR_TOWER, "A compact radar that can show a small portion of the map."},
    { GameObject::TYPE_RESEARCH_CENTER, "A building that will allow you to research and to develop new technologies and improvements."},
    { GameObject::TYPE_RES_GEN_ENERGY, "A generator of energy.\nConnect it to your base to produce energy."},
    { GameObject::TYPE_RES_GEN_ENERGY_SOLAR, "A solar panel.\nConnect it to your base to produce energy."},
    { GameObject::TYPE_RES_GEN_MATERIAL, "A generator of material.\nConnect it to your base to mine material."},
    { GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT, "A mine that can extract material from the ground.\n"
                                                 "Connect it to your base to produce material."},
    { GameObject::TYPE_RES_STORAGE_BLOBS, "Storage unit that can contain 100 units of blobs."},
    { GameObject::TYPE_RES_STORAGE_DIAMONDS, "Storage unit that can contain 150 units of diamonds."},
    { GameObject::TYPE_RES_STORAGE_ENERGY, "Storage unit that can contain 500 units of energy."},
    { GameObject::TYPE_RES_STORAGE_MATERIAL, "Storage unit that can contain 250 units of material."},
    { GameObject::TYPE_ROCKS, "Some rocks."},
    { GameObject::TYPE_TEMPLE, "An ancient temple that can be explored."},
    { GameObject::TYPE_TREES, "A single tree which will slowly grow into a forest."},
    { GameObject::TYPE_UNIT_MEDIC1, "This unit is specialized in healing other units.\nIt's a bit slow, but effective."},
    { GameObject::TYPE_UNIT_SCOUT1, "A light and fast unit ideal for exploring, but not for fighting."},
    { GameObject::TYPE_UNIT_SOLDIER1, "A basic soldier unit.\nUseful for defense and exploration."},
    { GameObject::TYPE_UNIT_SOLDIER2, "A slow, but versatile unit."},
    { GameObject::TYPE_UNIT_WORKER1, "A basic worker unit.\nIt is specialized in construction and conquest."},
    { GameObject::TYPE_WALL, "A defensive wall."},
    { GameObject::TYPE_WALL_GATE, "A gate that can be controlled to open a passage through a defensive wall."}
};

// -- OBJECT CATEGORY --
const GameObjectCategoryId GameObject::CAT_NULL = 0;

const GameObjectCategoryId GameObject::CAT_COLLECTABLE = h{}("COLLECTABLE");
const GameObjectCategoryId GameObject::CAT_GENERIC = h{}("GENERIC");
const GameObjectCategoryId GameObject::CAT_RES_GENERATOR = h{}("RES_GEN");
const GameObjectCategoryId GameObject::CAT_RES_STORAGE = h{}("RES_STORAGE");
const GameObjectCategoryId GameObject::CAT_SCENE_OBJ = h{}("SCENE_OBJ");
const GameObjectCategoryId GameObject::CAT_UNIT = h{}("UNIT");

// -- OBJECT VARIANT --
const GameObjectVariantId GameObject::VAR_0 = 0;

// -- CONSTRUCTOR & DESTRUCTOR --
GameObject::GameObject(GameObjectTypeId type, GameObjectCategoryId cat, int rows, int cols)
    : mIsoObj(new IsoObject(rows, cols))
    , mObjId(++counter)
    , mFaction(NO_FACTION)
    , mType(type)
    , mCategory(cat)
    , mRows(rows)
    , mCols(cols)
{
    // default colors to mark objects that haven't set any
    mObjColors.push_back(0xFFFFFFFF);
    mObjColors.push_back(0xFF00FFFF);
}

GameObject::~GameObject() { delete mIsoObj; }

void GameObject::OnPositionChanged() { }

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

int GameObject::GetRow0() const { return mCell->row; }
int GameObject::GetCol0() const { return mCell->col; }
int GameObject::GetRow1() const { return 1 + mCell->row - mRows; }
int GameObject::GetCol1() const { return 1 + mCell->col - mCols; }

void GameObject::SetFaction(PlayerFaction f)
{
    if(f == mFaction)
        return ;

    mFaction = f;

    OnFactionChanged();

    UpdateGraphics();
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
    const float delta = 0.01f;
    return  mHealth >= mMaxHealth || (mMaxHealth - mHealth) < delta;
}

void GameObject::SetHealth(float val)
{
    const float minDelta = 0.01f;
    const float oldH = mHealth;

    mHealth = val;

    if(mHealth > mMaxHealth || (mMaxHealth - mHealth) < minDelta)
        mHealth = mMaxHealth;
    else if(mHealth < 0.f)
        mHealth = 0.f;

    const float diff = std::fabs(mHealth - oldH);

    if(diff > minDelta)
        NotifyValueChanged();
}

void GameObject::SumHealth(float val)
{
    SetHealth(mHealth + val);
}

bool GameObject::IsEnergyMax() const
{
    const float delta = 0.01f;
    return  mEnergy >= mMaxEnergy || (mMaxEnergy - mEnergy) < delta;
}

void GameObject::SetEnergy(float val)
{
    const float minDelta = 0.01f;
    const float oldEn = mEnergy;

    mEnergy = val;

    if(mEnergy > mMaxEnergy || (mMaxEnergy - mEnergy) < minDelta)
        mEnergy = mMaxEnergy;
    else if(mEnergy < 0.f)
        mEnergy = 0.f;

    const float diff = std::fabs(mEnergy - oldEn);

    if(diff > minDelta)
        NotifyValueChanged();
}

void GameObject::SumEnergy(float val)
{
    SetEnergy(mEnergy + val);
}

bool GameObject::HasEnergyForActionStep(GameObjectActionType action) const
{
    if(action < NUM_OBJ_ACTIONS)
    {
        const float cost = ACTION_COSTS[action];

        if(mFaction != NO_FACTION)
        {
            Player * p = mScreen->GetGame()->GetPlayerByFaction(mFaction);

            return GetEnergy() >= cost && p->GetTurnEnergy() >= cost;
        }
        else
            return GetEnergy() >= cost;
    }
    else
        return false;
}

void GameObject::ActionStepCompleted(GameObjectActionType action)
{
    if(action < NUM_OBJ_ACTIONS)
    {
        // ENERGY
        const float costEnergy = -ACTION_COSTS[action];

        SumEnergy(costEnergy);

        if(mFaction != NO_FACTION)
        {
            Player * p = mScreen->GetGame()->GetPlayerByFaction(mFaction);
            p->SumTurnEnergy(costEnergy);
        }

        // EXPERIENCE
        SumExperience(ACTION_EXPERIENCE[action]);
    }
}

void GameObject::SetExperience(int val)
{
    if(val > mExpToNextLvl)
        val = mExpToNextLvl;

    if(val == mExp)
        return ;

    mExp = val;

    NotifyValueChanged();
}

void GameObject::SumExperience(int val)
{
    SetExperience(val + mExp);
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

void GameObject::Hit(float damage, PlayerFaction attacker)
{
    using namespace sgl::graphic;

    SumHealth(-damage);

    const int numPart0 = 20 * mRows * mCols;
    int numPart = numPart0;

    const int maxQuad = 4;
    int numQuad = 1;

    const float angInc = 90.f;
    float ang0 = 0.f;

    // hit, but alive -> pick random quadrant
    if(mHealth > 0.f)
    {
        const int quad0 = 0;
        sgl::utilities::UniformDistribution genQuad(quad0, maxQuad - 1);

        ang0 += angInc * genQuad.GetNextValue();
    }
    // hit and destroyed -> use all quadrants
    else
    {
        const int multPart = maxQuad;
        numPart *= multPart;

        numQuad = maxQuad;

        // record stats for players
        // NOTE register kills only when destroying enemies
        if(mFaction != NO_FACTION)
            mGameMap->RegisterEnemyKill(attacker);

        mGameMap->RegisterCasualty(mFaction);
    }

    float ang1 = ang0 + angInc;

    const int numPartQuad = numPart / numQuad;

    auto pu = static_cast<UpdaterDamage *>(GetScreen()->GetParticleUpdater(PU_DAMAGE));

    const unsigned int texInd = SpriteIdParticles::ID_PART_RECT_4x4;
    Texture * tex = TextureManager::Instance()->GetSprite(SpriteFileParticles, texInd);

    IsoObject * isoObj = GetIsoObject();
    const float objXC = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float objYC = isoObj->GetY() + isoObj->GetHeight() * 0.5f;

    // random generator of rotation angle
    const int minRot = 0;
    const int maxRot = 360;
    sgl::utilities::UniformDistribution genRot(minRot, maxRot);

    // random generator for velocity direction
    sgl::utilities::UniformDistribution genVel(static_cast<int>(ang0), static_cast<int>(ang1));

    const float deg2rad = sgl::core::Math::PIf / 180.f;

    // random generator for speed
    const int minSpeed = 100;
    const int maxSpeed = 300;

    sgl::utilities::UniformDistribution genSpeed(minSpeed, maxSpeed);

    // random generator for decay speed
    const int minDecSpeed = 250;
    const int maxDecSpeed = 500;
    sgl::utilities::UniformDistribution genDecSpeed(minDecSpeed, maxDecSpeed);

    // random generator for scale
    const int minScale = 1;
    const int maxScale = 2;
    sgl::utilities::UniformDistribution genScale(minScale, maxScale);

    // random generator for color
    const int color0 = 0;
    const int colorN = mObjColors.size() - 1;

    sgl::utilities::UniformDistribution genColor(color0, colorN);

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
}

void GameObject::SetActiveActionToDefault() { mActiveAction = IDLE; }

void GameObject::OnNewTurn(PlayerFaction faction)
{
    if(mFaction == faction)
        RestoreTurnEnergy();
}

void GameObject::Update(float) { }

void GameObject::OnFactionChanged() { }
void GameObject::OnLinkedChanged() { }

void GameObject::SetDefaultColors()
{
    // clear current colors
    mObjColors.clear();

    // assign new colors based on faction
    switch(mFaction)
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

void GameObject::NotifyValueChanged()
{
    for(const auto & it : mOnValueChanged)
        it.second();
}

void GameObject::RestoreTurnEnergy()
{
    const float basePerc = 0.5f;
    const float baseEnergy = mMaxEnergy * basePerc;
    const float newEnergy = (mMaxEnergy - baseEnergy) * mEnergyRegPower;
    const float prevEnergy = mEnergy * mEnergyRegPower;
    const float energy = std::roundf(baseEnergy + prevEnergy + newEnergy);

    SumEnergy(energy);
}

} // namespace game
