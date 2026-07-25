#pragma once

#include "GameObjects/GameObjectsGroup.h"

namespace game
{

class GameMap;
class Player;

class CityGroup : public GameObjectsGroup
{
public:
    CityGroup(GameMap * gm);

    bool Save(sgl::utilities::BinaryFile & bf) const override;

    bool IsCityConquered() const;

    void UpdateCityConquered(Player * conqueror);

private:
    GameMap * mGameMap = nullptr;
    bool mConquered = false;

};

inline CityGroup::CityGroup(GameMap * gm) : mGameMap(gm) {}

inline bool CityGroup::IsCityConquered() const { return mConquered; }

} // namespace game
