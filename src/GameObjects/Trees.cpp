#include "GameObjects/Trees.h"

#include "GameData.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/UniformDistribution.h>

namespace
{
    constexpr float treeHealth = 40.f;
}

namespace game
{

Trees::Trees(GameObjectVariantId var)
    : GameObject(TYPE_TREES, CAT_SCENE_OBJ, 1, 1)
{
    mVariant = var;

    SetStatic(true);

    // health
    SetMaxHealth(treeHealth);
    SetHealth(treeHealth);

    // randomize turns for change
    const int minTurns = 10;
    const int maxTurns = 40;
    sgl::utilities::UniformDistribution dis(minTurns, maxTurns);
    mTurnsToChange = dis.GetNextValue();

    SetObjColors();
    SetImage();
}

void Trees::OnNewTurn(PlayerFaction)
{
    ++mTurns;

    // needs more turns...
    if(mTurns < mTurnsToChange)
        return ;

    // CHANGE TIME!
    mTurns = 0;

    // still growing 1 tree
    if(1 == mNumTrees && mVariant < (NUM_TREE1_VARIANTS - 1))
    {
        ++mVariant;

        SetImage();
    }
    // grow more trees
    else if(mNumTrees < MAX_TREE1_TREES)
    {
        // randomize new variant
        sgl::utilities::UniformDistribution dis(0, NUM_TREE1_VARIANTS - 1);
        mVariant = dis.GetNextValue();

        ++mNumTrees;

        SetImage();

        SetMaxHealth(GetMaxHealth() + treeHealth);
        SumHealth(treeHealth);
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

    sgl::utilities::UniformDistribution dis(0, NUM_TREE1_VARIANTS - 1);
    const int variant = dis.GetNextValue();

    gm->CreateObject(MapLayers::OBJECTS1, GameObject::TYPE_TREES, variant, NO_FACTION, r0, c0, false);

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

    const unsigned int spriteId0 = IsSelected() ? TREE1_1T_1_SEL : TREE1_1T_1;
    const unsigned int baseSpriteId = spriteId0 + (NUM_TREE1_VARIANTS * (mNumTrees - 1));
    const unsigned int spriteId = baseSpriteId + mVariant;
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileTrees, spriteId);

    isoObj->SetTexture(tex);
}

void Trees::SetObjColors()
{
    mObjColors.clear();

    mObjColors.push_back(0x5d614bff);
    mObjColors.push_back(0x858a6bff);
    mObjColors.push_back(0x787d61ff);
    mObjColors.push_back(0x515441ff);
    mObjColors.push_back(0x6c7057ff);
    mObjColors.push_back(0x454838ff);
}

} // namespace game
