#include "GameObjects/DefensiveTower.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "GameObjects/ObjectData.h"
#include "Particles/DataParticleSingleLaser.h"
#include "Particles/UpdaterSingleLaser.h"
#include "Screens/ScreenGame.h"

#include <sgl/core/Math.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

const int maxAttVal = 11;
const int attRanges[maxAttVal] = { 0, 2, 3, 4, 5, 6, 7, 9, 10, 12, 14 };

DefensiveTower::DefensiveTower(const ObjectData & data)
    : Structure(TYPE_DEFENSIVE_TOWER, CAT_GENERIC, data.GetRows(), data.GetCols())
{
    // set attack range converting attribute
    mAttackRange = attRanges[data.GetAttributes()[OBJ_ATT_FIRE_RANGE]];

    SetImage();
}

void DefensiveTower::Update(float delta)
{
    // do nothing if not linked
    if(!IsLinked())
    {
        mTarget = nullptr;
        return ;
    }

    // check if there's any target in range
    CheckForEnemies();

    // attacking other object
    if(mTarget)
    {
        mTimerAttack -= delta;

        // time to shoot!
        if(mTimerAttack < 0.f)
        {
            // target still alive -> shoot
            if(GetGameMap()->HasObject(mTarget))
                Shoot();
            // target destroyed -> clear pointer
            else
                mTarget = nullptr;

            mTimerAttack = mTimeAttack;
        }
    }
}

void DefensiveTower::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void DefensiveTower::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_DTOWER_L1;

    if(NO_FACTION == faction)
        texInd += sel;
    else
        texInd = ID_STRUCT_DTOWER_L1_F1 + (faction * NUM_DTOWER_SPRITES_PER_FAC) + sel;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

void DefensiveTower::CheckForEnemies()
{
    const GameMap * gm = GetGameMap();

    const int row = GetRow0();
    const int col = GetCol0();

    const int mapRows = gm->GetNumRows();
    const int mapCols = gm->GetNumCols();

    const PlayerFaction faction = GetFaction();

    std::vector<GameObject *> objs;

    // find all enemies in range
    for(int i = 1; i <= mAttackRange; ++i)
    {
        const int rTL = (row - i) > 0 ? (row - i) : 0;
        const int rBR = (row + i) < mapRows ? (row + i) : (mapRows - 1);

        const int cTL = (col - i) > 0 ?  (col - i) : 0;
        const int cBR = (col + i) < mapCols ? (col + i) : (mapCols - 1);

        for(int r = rTL; r <= rBR; ++r)
        {
            for(int c = cTL; c <= cBR; ++c)
            {
                const GameMapCell & cell = gm->GetCell(r, c);

                GameObject * obj = cell.objTop;

                // only visible enemy objects
                if(obj && obj->GetFaction() != NO_FACTION && obj->GetFaction() != faction && obj->IsVisible())
                    objs.push_back(cell.objTop);
            }
        }
    }

    // no potential target -> clear current
    if(objs.empty())
    {
        mTarget = nullptr;
        return ;
    }

    // chose target
    // TODO more complex logic, for now just selecting 1st one
    mTarget = objs.front();
}

void DefensiveTower::Shoot()
{
    using namespace sgl::graphic;
    // TODO calculate chance of hitting based on attack and defense attributes
    // for now assuming it's always hit

    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    auto pu = static_cast<UpdaterSingleLaser *>(GetScreen()->GetParticleUpdater(PU_SINGLE_LASER));

    const unsigned int texInd = SpriteIdUnitsParticles::IND_UPAR_LASER_F1 + faction;
    Texture * tex = TextureManager::Instance()->GetSprite(SpriteFileUnitsParticles, texInd);

    IsoObject * isoObj = GetIsoObject();
    IsoObject * isoTarget = mTarget->GetIsoObject();

    const float isoX = isoObj->GetX();
    const float isoXC = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float isoY = isoObj->GetY();
    const float isoTargetX = isoTarget->GetX();
    const float isoTargetY = isoTarget->GetY();
    const float x0 = isoTargetX < isoX ? isoXC - 20.f : isoXC + 20.f;
    const float y0 = isoTargetY < isoY ? isoY + 4 : isoY + 30;
    const float tX = isoTarget->GetX() + (isoTarget->GetWidth() - tex->GetWidth()) * 0.5f;
    const float tY = isoTargetY + (isoTarget->GetHeight() - tex->GetHeight()) * 0.5f;
    const float speed = 300.f;

    const float rad2deg = 180.f / sgl::core::Math::PIf;
    const float dy0 = tY - y0;
    const float dx1 = tX - x0;
    const float dy1 = dy0;
    const float s = dy0 / sqrtf(dx1 * dx1 + dy1 * dy1);
    const float as = asinf(s);
    const double angleDeg = as * rad2deg;
    double angle;

    if(dx1 < 0.f)
    {
        // bottom left
        if(dy1 > 0.f)
            angle = 180.f - angleDeg;
        // top left
        else
            angle = 180.f - angleDeg;
    }
    else
    {
        // bottom right
        if(dy1 > 0.f)
            angle = angleDeg;
        // top right
        else
            angle = 360.f + angleDeg;
    }

    const DataParticleSingleLaser pd =
    {
        tex,
        GetGameMap(),
        mTarget,
        angle,
        x0,
        y0,
        tX,
        tY,
        speed,
        mWeaponDamage,
        GetFaction()
    };

    pu->AddParticle(pd);
}

} // namespace game
