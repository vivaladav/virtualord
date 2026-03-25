#include "Structure.h"

#include "GameConstants.h"
#include "IsoObject.h"
#include "Player.h"
#include "Widgets/BlinkingIconEnergy.h"

namespace game
{

Structure::Structure(const ObjectData & data, const ObjectInitData & initData)
    : GameObject(data, initData)
    , mIconEnergy(new BlinkingIconEnergy)
{
    SetStructure(true);
    SetStatic(true);

    if(IsFactionLocal())
        ShowIconEnergy();
    else
        HideIconEnergy();
}

Structure::~Structure()
{
    delete mIconEnergy;
}

void Structure::OnNewTurn(PlayerFaction faction)
{
    GameObject::OnNewTurn(faction);

    if(faction != GetFaction())
        return ;

    if(!IsLinked())
        return ;

    ConsumeResources();
    ProduceResources();
}

float Structure::GetTimeBuildUnit() const
{
    const float maxTime = 5.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_CONSTRUCTION));
}

void Structure::OnPositionChanged()
{
    PositionIconEnergy();
}

int Structure::GetResourceProduction(ExtendedResource res) const
{
    return 0;
}

int Structure::GetResourceUsage(ExtendedResource) const
{
    return 0;
}

void Structure::OnFactionChanged()
{
    GameObject::OnFactionChanged();

    if(GetFaction() != NO_FACTION && !IsLinked())
        ShowIconEnergy();
}

void Structure::OnLinkedChanged()
{
    GameObject::OnLinkedChanged();

    if(IsLinked())
        HideIconEnergy();
    else if(GetFaction() != NO_FACTION)
        ShowIconEnergy();
}

void Structure::HideIconEnergy()
{
    mIconEnergy->SetVisible(false);
    mIconEnergy->SetEnabled(false);
}

void Structure::ShowIconEnergy()
{
    if(!IsFactionLocal())
        return ;

    mIconEnergy->SetVisible(true);
    mIconEnergy->SetEnabled(true);

    PositionIconEnergy();
}

void Structure::PositionIconEnergy()
{
    const auto isoObj = GetIsoObject();
    const int isoX = isoObj->GetX();
    const int isoY = isoObj->GetY();
    const int isoW = isoObj->GetWidth();

    const int iconMarginV = 5;
    const int iconX = isoX + (isoW - mIconEnergy->GetWidth()) / 2;
    const int iconY = isoY - mIconEnergy->GetHeight() - iconMarginV;

    mIconEnergy->SetPosition(iconX, iconY);
}

void Structure::ProduceResources()
{
    auto p = GetOwner();

    if(p == nullptr)
        return ;

    const Player::Stat statIds[] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
        Player::MONEY,
        Player::RESEARCH
    };

    static_assert((sizeof(statIds) / sizeof(Player::Stat)) == NUM_EXTENDED_RESOURCES);

    for(unsigned int i = 0; i < NUM_EXTENDED_RESOURCES; ++i)
    {
        const int val = GetResourceProduction(static_cast<ExtendedResource>(i));

        if(val > 0)
            p->SumResource(statIds[i], val);
    }
}

void Structure::ConsumeResources()
{
    auto p = GetOwner();

    if(p == nullptr)
        return ;

    const Player::Stat statIds[] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
        Player::MONEY,
        Player::RESEARCH
    };

    static_assert((sizeof(statIds) / sizeof(Player::Stat)) == NUM_EXTENDED_RESOURCES);

    for(unsigned int i = 0; i < NUM_EXTENDED_RESOURCES; ++i)
    {
        const int val = GetResourceUsage(static_cast<ExtendedResource>(i));

        if(val > 0)
            p->SumResource(statIds[i], -val);
    }
}

} // namespace game
