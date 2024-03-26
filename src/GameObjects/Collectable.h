#pragma once

#include "GameObject.h"

namespace game
{

class CollectableGenerator;
class Player;

class Collectable : public GameObject
{
public:
    Collectable(GameObjectTypeId type, int rows, int cols);

    virtual void Collected(Player * collector);
};

inline Collectable::Collectable(GameObjectTypeId type, int rows, int cols) :
    GameObject(type, GameObject::CAT_COLLECTABLE, rows, cols)
{
}

} // namespace game
