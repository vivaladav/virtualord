#pragma once

#include "GameObject.h"

namespace game
{

class CollectableGenerator;

class Collectable : public GameObject
{
public:
    Collectable(GameObjectTypeId type, int rows, int cols);

    virtual void Collected();
};

inline Collectable::Collectable(GameObjectTypeId type, int rows, int cols) :
    GameObject(type, GameObject::CAT_COLLECTABLE, rows, cols)
{
}

} // namespace game
