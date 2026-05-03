#include "GameObjects/Trees.h"

#include "Game.h"
#include "GameData.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/UniformDistribution.h>

namespace
{
    constexpr float maxTreeEnergy = 100.f;
    constexpr float maxTreeHealth = 200.f;
}

namespace game
{

Trees::Trees(const ObjectData & data, const ObjectInitData & initData, GameObjectVariantId var)
    : GameObject(data, initData)
{
    const GameObjectTypeId type = GetObjectType();

    if(type == ObjectData::TYPE_TREES2)
    {
        mNumVariants = NUM_TREE2_VARIANTS;
        mMaxNum = MAX_TREE2_TREES;
    }
    else if(type == ObjectData::TYPE_TREES3)
    {
        mNumVariants = NUM_TREE3_VARIANTS;
        mMaxNum = MAX_TREE3_TREES;
    }
    else
    {
        mNumVariants = NUM_TREE1_VARIANTS;
        mMaxNum = MAX_TREE1_TREES;
    }

    mVariant = var;

    SetStatic(true);

    // health
    SetMaxHealth(maxTreeHealth);

    // energy
    SetMaxEnergy(maxTreeEnergy);

    // randomize turns for change
    const int minTurns = 10;
    const int maxTurns = 25;
    sgl::utilities::UniformDistribution dis(minTurns, maxTurns, GetGame()->GetRandSeed());
    mTurnsToChange = dis.GetNextValue();

    SetObjColors();
    SetImage();
}

void Trees::OnNewTurn(PlayerFaction faction)
{
    GameObject::OnNewTurn(faction);

    ++mTurns;

    // needs more turns...
    if(mTurns < mTurnsToChange)
        return ;

    // CHANGE TIME!
    mTurns = 0;

    // still growing 1 tree
    if(1 == mNumTrees && mVariant < (mNumVariants - 1))
    {
        ++mVariant;

        SetImage();
    }
    // grow more trees
    else if(mNumTrees < mMaxNum)
    {
        // randomize new variant
        sgl::utilities::UniformDistribution dis(0, mNumVariants - 1, GetGame()->GetRandSeed());
        mVariant = dis.GetNextValue();

        ++mNumTrees;

        SetImage();

        // health
        SetMaxHealth(maxTreeHealth * mNumTrees);

        // increase total energy
        SetMaxEnergy(maxTreeEnergy * mNumTrees);
    }
    // spawn tree
    else
    {
        GameMap * gm = GetGameMap();

        const int r0 = GetRow0();
        const int c0 = GetCol0();
        const int rows = gm->GetNumRows();
        const int cols = gm->GetNumCols();

        const int rTL = (r0 > 0) ? r0 - 1 : r0;
        const int cTL = (c0 > 0) ? c0 - 1 : c0;
        const int rBR = (r0 < (rows - 1)) ? r0 + 1 : r0;
        const int cBR = (c0 < (cols -1)) ? c0 + 1 : c0;

        for(int r = rTL; r <= rBR; ++r)
        {
            const int ind0 = r * cols;

            for(int c = cTL; c <= cBR; ++c)
            {
                const int ind = ind0 + c;

                if(!gm->HasObject(ind))
                {
                    SpawnTree(r, c);

                    return;
                }
            }
        }
    }
}

void Trees::SpawnTree(int r0, int c0)
{
    GameMap * gm = GetGameMap();

    sgl::utilities::UniformDistribution dis(0, mNumVariants - 1, GetGame()->GetRandSeed());
    const int variant = dis.GetNextValue();

    gm->CreateObject(MapLayers::REGULAR_OBJECTS, GetObjectType(), variant, NO_FACTION, r0, c0, false);

    // set cell type of new tree
    gm->SetCellType(r0, c0, TREES1);

    // set cell type of surrounding cells
    const int rows = gm->GetNumRows();
    const int cols = gm->GetNumCols();
    const int rTL = (r0 > 0) ? r0 - 1: r0;
    const int cTL = (c0 > 0) ? c0 - 1: c0;
    const int rBR = (r0 < (rows - 1)) ? r0 + 1 : r0;
    const int cBR = (c0 < (cols -1)) ? c0 + 1 : c0;

    for(int r = rTL; r <= rBR; ++r)
    {
        const int ind0 = r * cols;

        for(int c = cTL; c <= cBR; ++c)
        {
            const int ind = ind0 + c;

            const GameMapCell & cell = gm->GetCell(ind);

            if(!GameMapCell::IsTypePrimary(cell.basicType))
            {
                gm->SetCellType(r, c, TREES1_SURR);
                gm->UpdateCellType(ind, cell);
            }
        }
    }
}

void Trees::UpdateGraphics()
{
    SetImage();

    SetObjColors();
}

void Trees::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    // set texture
    auto tm = sgl::graphic::TextureManager::Instance();

    const GameObjectTypeId type = GetObjectType();

    unsigned int spriteId0 = TREE1_1T_1;

    if(type == ObjectData::TYPE_TREES2)
        spriteId0 = TREE2_1T_1;
    else if(type == ObjectData::TYPE_TREES3)
        spriteId0 = TREE3_1T_1;

    const unsigned int baseSpriteId = spriteId0 + (mNumVariants * (mNumTrees - 1));
    const unsigned int spriteId = baseSpriteId + mVariant;
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileTrees, spriteId);

    isoObj->SetTexture(tex);
}

void Trees::SetObjColors()
{
    mObjColors.clear();

    const GameObjectTypeId type = GetObjectType();

    if(type == ObjectData::TYPE_TREES2)
    {
        mObjColors.push_back(0x7d9056ff);
        mObjColors.push_back(0x617043ff);
        mObjColors.push_back(0x6b6548ff);
        mObjColors.push_back(0x4d4933ff);
        mObjColors.push_back(0x35392dff);
        mObjColors.push_back(0x2b2922ff);
    }
    else if(type == ObjectData::TYPE_TREES3)
    {
        mObjColors.push_back(0x96a96fff);
        mObjColors.push_back(0x7d9056ff);
        mObjColors.push_back(0x536039ff);
        mObjColors.push_back(0xa3b87aff);
        mObjColors.push_back(0x37392dff);
        mObjColors.push_back(0x727255ff);
        mObjColors.push_back(0x464a36ff);
    }
    else
    {
        mObjColors.push_back(0x5d614bff);
        mObjColors.push_back(0x858a6bff);
        mObjColors.push_back(0x787d61ff);
        mObjColors.push_back(0x515441ff);
        mObjColors.push_back(0x6c7057ff);
        mObjColors.push_back(0x454838ff);
    }
}

} // namespace game
