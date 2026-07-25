#include "GameObjects/GameObjectsGroup.h"

#include "GameObjects/GameObject.h"

#include <sgl/utilities/BinaryFile.h>

#include <algorithm>

namespace game
{

bool GameObjectsGroup::Save(sgl::utilities::BinaryFile & bf) const
{
    bf.WriteUint(mObjects.size());

    for(const GameObject * obj : mObjects)
        bf.WriteUint(obj->GetObjectId());

    return true;
}

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

void GameObjectsGroup::SetSelected(bool selected, OverlaySelection * overlay) const
{
    for(auto o : mObjects)
        o->SetSelected(selected, overlay);
}

} // namespace game
