#pragma once

#include "GameObjects/GameObjectsGroup.h"

namespace game
{

class Player;

class CityGroup : public GameObjectsGroup
{
public:
    CityGroup(GameMap * gm);

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    bool IsCityConquered() const;

    void UpdateCityConquered(Player * conqueror);

private:
    bool mConquered = false;

};

inline CityGroup::CityGroup(GameMap * gm) : GameObjectsGroup(gm) {}

inline bool CityGroup::IsCityConquered() const { return mConquered; }

} // namespace game
