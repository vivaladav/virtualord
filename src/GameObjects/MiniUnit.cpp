#include "GameObjects/MiniUnit.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "GameObjectTools/Weapon.h"

#include <sgl/core/Point.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/BinaryFile.h>
#include <sgl/utilities/UniformDistribution.h>

namespace
{
    const float maxSingleHealthValue = 50.f;
    const float maxSingleEnergyValue = 200.f;

    const int MAX_ELEMENTS = 5;
}

namespace game
{

MiniUnit::MiniUnit(const ObjectData & data, const ObjectInitData & initData, int elements)
    : GameObject(data, initData)
{
    SetObjectVariant(elements);

    // set actual speed
    const float maxSpeed = 10.f;
    SetMaxSpeed(maxSpeed);

    // health
    const float maxHealthValue = maxSingleHealthValue * elements;
    SetMaxHealth(maxHealthValue);

    // energy
    const float maxEnergy = maxSingleEnergyValue * elements;
    SetMaxEnergy(maxEnergy);

    // INIT GRAPHICS
    SetImage();
}

bool MiniUnit::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = GameObject::Load(bf);

    if(!res)
        return false;

    // flags
    mMoving = bf.ReadBool();
    mTargetReached = bf.ReadBool();

    return true;
}

bool MiniUnit::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = GameObject::Save(bf);

    if(!res)
        return false;

    // flags
    bf.WriteBool(mMoving);
    bf.WriteBool(mTargetReached);

    return true;
}

void MiniUnit::SetNumElements(int num)
{
    // cap elements
    if(num > MAX_ELEMENTS)
        num = MAX_ELEMENTS;

    // nothing has changed
    if(num == GetObjectVariant())
        return ;

    SetObjectVariant(num);

    // health
    const float maxHealthValue = maxSingleHealthValue * num;
    SetMaxHealth(maxHealthValue);

    // energy
    const float maxEnergy = maxSingleEnergyValue * num;
    SetMaxEnergy(maxEnergy);

    UpdateGraphics();
}

void MiniUnit::Update(float delta)
{
    if(mTargetReached)
    {
        const GameObjectTypeId type = GetObjectType();

        if(type == ObjectData::TYPE_MINI_UNIT1)
            ExplodeNearEnemy();

        // reset flag
        mTargetReached = false;
    }

    // ATTACKING OTHER OBJECTS
    if(mWeapon != nullptr)
    {
        mWeapon->Update(delta);

        if(mWeapon->IsReadyToShoot())
            PrepareShoot();
    }
}

void MiniUnit::UpdateGraphics()
{
    SetImage();
}

void MiniUnit::SetImage()
{
    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    unsigned int texInd0 = SID_MUNIT_01_1X_F1;

    if(GetObjectType() == ObjectData::TYPE_MINI_UNIT2)
        texInd0 = SID_MUNIT_02_1X_F1;

    const unsigned int texInd = texInd0 + (NUM_MUNIT_SPRITES_PER_FACTION * faction) +
                                (GetObjectVariant() - 1);

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex =tm->GetSprite(SpriteFileMiniUnits, texInd);

    GetIsoObject()->SetTexture(tex);
}

void MiniUnit::ExplodeNearEnemy()
{
    auto gm = GetGameMap();

    const int mapRows = gm->GetNumRows();
    const int mapCols = gm->GetNumCols();

    const std::vector<GameMapCell> & cells = gm->GetCells();

    const int cr = GetRow0();
    const int cc = GetCol0();

    const int rad = 1;
    const int r0 = cr >= rad ? cr - rad : 0;
    const int r1 = cr + rad < mapRows ? cr + rad + 1 : mapRows;
    const int c0 = cc >= rad ? cc - rad : 0;
    const int c1 = cc + rad < mapCols ? cc + rad + 1 : mapCols;

    const PlayerFaction ownFaction = GetFaction();

    for(int r = r0; r < r1; ++r)
    {
        const int ind0 = r * mapCols;

        for(int c = c0; c < c1; ++c)
        {
            const int ind = ind0 + c;
            const GameMapCell & cell = cells[ind];

            // enemy found -> boom!
            if((cell.objTop != nullptr && cell.objTop->GetFaction() != ownFaction &&
                 cell.objTop->GetFaction() != NO_FACTION) ||
                (cell.objBottom != nullptr && cell.objBottom->GetFaction() != ownFaction &&
                 cell.objBottom->GetFaction() != NO_FACTION))
            {
                SelfDestroy();
                return ;
            }
        }
    }
}

void MiniUnit::PrepareShoot()
{
    using namespace sgl;

    const IsoObject * isoObj = GetIsoObject();
    const int numElements = GetObjectVariant();

    core::Pointd2D delta;

    if(1 == numElements)
        delta = {48, 9};
    else
    {
        std::vector<core::Pointd2D> deltas;

        if(2 == numElements)
        {
            deltas.emplace_back(24, 9);
            deltas.emplace_back(72, 9);
        }
        else if(3 == numElements)
        {
            deltas.emplace_back(16, 15);
            deltas.emplace_back(48, 15);
            deltas.emplace_back(80, 15);
        }
        else if(4 == numElements)
        {
            deltas.emplace_back(48, 0);
            deltas.emplace_back(16, 15);
            deltas.emplace_back(80, 15);
            deltas.emplace_back(48, 30);
        }
        else
        {
            deltas.emplace_back(48, 0);
            deltas.emplace_back(16, 15);
            deltas.emplace_back(48, 15);
            deltas.emplace_back(80, 15);
            deltas.emplace_back(48, 30);
        }

        utilities::UniformDistribution ud(0, deltas.size() - 1, GetGame()->GetRandSeed());
        delta = deltas[ud.GetNextValue()];
    }

    const float x0 = isoObj->GetX() + delta.x;
    const float y0 = isoObj->GetY() + delta.y;

    mWeapon->Shoot(x0, y0);
}

} // namespace game
