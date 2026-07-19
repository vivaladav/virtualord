#pragma once

#include<functional>
#include <vector>

namespace game
{

class GameObject;
class OverlaySelection;

class GameObjectsGroup
{
public:
    virtual ~GameObjectsGroup() = default;

    // basic objects handling
    void AddObject(GameObject * o);
    void RemoveObject(GameObject * o);

    unsigned int GetNumObjects() const;
    bool IsEmpty() const;

    // actions on objects
    void DoForAll(const std::function<void(GameObject *)> & f) const;
    void DoForObject(unsigned int index, const std::function<void(GameObject *)> & f) const;

    void SetSelected(bool selected, OverlaySelection * overlay) const;

protected:
    const std::vector<GameObject *> & GetObjects() const;

private:
    std::vector<GameObject *> mObjects;
};

inline void GameObjectsGroup::AddObject(GameObject * o) { mObjects.emplace_back(o); }

inline unsigned int GameObjectsGroup::GetNumObjects() const { return mObjects.size(); }
inline bool GameObjectsGroup::IsEmpty() const { return mObjects.empty(); }

inline const std::vector<GameObject *> & GameObjectsGroup::GetObjects() const
{
    return mObjects;
}

} // namespace game
