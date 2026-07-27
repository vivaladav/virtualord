#pragma once

#include "GameObjects/CollectableGenerator.h"

namespace game
{

class GameMap;

class DiamondsGenerator : public CollectableGenerator
{
public:
    DiamondsGenerator(const Game * g, GameMap * gm);
};

} // namespace game
