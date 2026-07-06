#include "GameObjects/GameObjectsGroup.h"

#include "GameObjects/GameObject.h"

#include <algorithm>

namespace game
{

void GameObjectsGroup::RemoveObject(GameObject * o)
{
    auto it = std::find(mObjects.begin(), mObjects.end(), o);

    if(it != mObjects.end())
        mObjects.erase(it);
}

void GameObjectsGroup::DoForAll(const std::function<void(GameObject *)> & f) const
{
    for(auto o : mObjects)
        f(o);
}

void GameObjectsGroup::DoForObject(unsigned int index,
                                   const std::function<void(GameObject *)> & f) const
{
    if(index < mObjects.size())
        f(mObjects[index]);
}

void GameObjectsGroup::SetSelected(bool selected) const
{
    for(auto o : mObjects)
        o->SetSelected(selected);
}

} // namespace game
