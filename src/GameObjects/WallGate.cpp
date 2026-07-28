#include "GameObjects/WallGate.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "Player.h"

#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/BinaryFile.h>

namespace game
{

WallGate::WallGate(const ObjectData & data, const ObjectInitData & initData,
                   GameObjectVariantId orientation)
    : Structure(data, initData)
{
    mVariant = orientation;

    SetImage();
}

WallGate::~WallGate()
{
    auto p = GetOwner();

    if(p == nullptr)
        return ;

    p->SetCellWalkable(GetRow0(), GetCol0(), false);
}

bool WallGate::Load(sgl::utilities::BinaryFile & bf)
{
    const bool res = Structure::Load(bf);

    if(!res)
        return false;

    // flags
    mOpen = bf.ReadBool();

    return true;
}

bool WallGate::Save(sgl::utilities::BinaryFile & bf) const
{
    const bool res = Structure::Save(bf);

    if(!res)
        return false;

    // flags
    bf.WriteBool(mOpen);

    return true;
}

bool WallGate::Toggle()
{
    if(!IsLinked())
        return false;

    // toggle open value
    mOpen = !mOpen;

    // play SFX
    if(IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

        if(mOpen)
            ap->PlaySound("game/gate_open.ogg");
        else
            ap->PlaySound("game/gate_close.ogg");
    }

    // set cell walkable if open and not walkable otherwise
    const GameMapCell * cell = GetCell();
    GetGameMap()->SetCellWalkable(cell->row, cell->col, mOpen);

    ActionStepCompleted(TOGGLE_GATE);

    // update image
    SetImage();

    return true;
}

void WallGate::OnNewTurn(PlayerFaction faction)
{
    Structure::OnNewTurn(faction);

    if(faction != GetFaction())
        return ;

    // close gate if open and nothing on top
    if(IsOpen() && GetCell()->objTop == nullptr)
        GetGameMap()->CloseGate(this);
}

unsigned int WallGate::GetCostEnergy(unsigned int level)
{
    const unsigned int cost0 = 5;
    return (level + 1) * cost0;
}

unsigned int WallGate::GetCostMaterial(unsigned int level)
{
    const unsigned int cost0 = 10;
    return (level + 1) * cost0;
}

void WallGate::OnLinkedChanged()
{
    Structure::OnLinkedChanged();

    auto p = GetOwner();

    if(p == nullptr)
        return ;

    p->SetCellWalkable(GetRow0(), GetCol0(), IsLinked());
}

void WallGate::UpdateGraphics()
{
    SetImage();
}

void WallGate::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    // set texture
    const int ind0 = mVariant == HORIZ ? WALL_GATE_L1_F1_HORIZ_CLOSED : WALL_GATE_L1_F1_VERT_CLOSED;
    const int ind1 = ind0 + static_cast<int>(mOpen);
    const int ind = ind1 + NUM_SPRITES_PER_WALL_GATE * faction;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, ind);
    isoObj->SetTexture(tex);
}

} // namespace game
