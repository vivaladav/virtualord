#include "Structure.h"

#include "GameConstants.h"
#include "IsoObject.h"
#include "Widgets/BlinkingIconEnergy.h"

namespace game
{

Structure::Structure(const ObjectData & data, const ObjectInitData & initData)
    : GameObject(data, initData)
    , mIconEnergy(new BlinkingIconEnergy)
{
    SetStructure(true);
    SetStatic(true);

    // init resource usage
    mResUsage.assign(NUM_EXTENDED_RESOURCES, 0);

    if(IsFactionLocal())
        ShowIconEnergy();
    else
        HideIconEnergy();
}

Structure::~Structure()
{
    delete mIconEnergy;
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

int Structure::GetResourceUsage(ExtendedResource res) const
{
    if(res < NUM_EXTENDED_RESOURCES)
        return mResUsage[res];
    else
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

void Structure::SetResourceUsage(ExtendedResource res, int val)
{
    if(res < NUM_EXTENDED_RESOURCES)
        mResUsage[res] = val;
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

} // namespace game
