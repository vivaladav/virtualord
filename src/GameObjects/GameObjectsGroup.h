#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <functional>
#include <vector>

namespace sgl { namespace utilities { class BinaryFile; } }

namespace game
{

class GameMap;
class GameObject;
class OverlaySelection;

class GameObjectsGroup
{
public:
    GameObjectsGroup(GameMap * gameMap);
    virtual ~GameObjectsGroup() = default;

    virtual bool Load(sgl::utilities::BinaryFile & bf);
    virtual bool Save(sgl::utilities::BinaryFile & bf) const;

    // basic objects handling
    void AddObject(GameObject * o);
    void RemoveObject(GameObject * o);

    unsigned int GetNumObjects() const;
    bool IsEmpty() const;

    GameObjectTypeId GetObjectsType() const;

    // actions on objects
    void DoForAll(const std::function<void(GameObject *)> & f) const;
    void DoForObject(unsigned int index, const std::function<void(GameObject *)> & f) const;

    void SetSelected(bool selected, OverlaySelection * overlay) const;

protected:
    GameMap * GetGameMap() const;

    const std::vector<GameObject *> & GetObjects() const;

private:
    std::vector<GameObject *> mObjects;

    GameMap * mGameMap = nullptr;
};

inline GameObjectsGroup::GameObjectsGroup(GameMap * gm) : mGameMap(gm) {}

inline void GameObjectsGroup::AddObject(GameObject * o) { mObjects.emplace_back(o); }

inline unsigned int GameObjectsGroup::GetNumObjects() const { return mObjects.size(); }
inline bool GameObjectsGroup::IsEmpty() const { return mObjects.empty(); }

inline GameMap * GameObjectsGroup::GetGameMap() const { return mGameMap; }

inline const std::vector<GameObject *> & GameObjectsGroup::GetObjects() const
{
    return mObjects;
}

} // namespace game
