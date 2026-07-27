#pragma once

#include "Cell2D.h"
#include "GameObjects/GameObjectsGroup.h"

#include <vector>

namespace game
{

class GameObject;

enum PlayerFaction : unsigned int;

class MiniUnitsGroup : public GameObjectsGroup
{
public:
    MiniUnitsGroup(GameMap * gameMap);

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    void SetFaction(PlayerFaction faction);
    PlayerFaction GetFaction() const;

    bool CanAttack() const;

    // PATH
    void ClearPath();
    const std::vector<unsigned int> & GetPath() const;
    void SetPath(const std::vector<unsigned int> && path);
    void SetTarget(const Cell2D & target);

    bool PopPathTargetBack(int mapCols);

    bool HasPathSet() const;

    const Cell2D & GetPathTarget() const;

private:
    std::vector<unsigned int> mPath;

    Cell2D mTarget;

    PlayerFaction mFaction;
};

inline void MiniUnitsGroup::SetFaction(PlayerFaction faction) { mFaction = faction; }
inline PlayerFaction MiniUnitsGroup::GetFaction() const { return mFaction; }

inline void MiniUnitsGroup::ClearPath() { mPath.clear(); }

inline const std::vector<unsigned int> & MiniUnitsGroup::GetPath() const
{
    return mPath;
}

inline void MiniUnitsGroup::SetPath(const std::vector<unsigned int> && path)
{
    mPath = path;
}

inline void MiniUnitsGroup::SetTarget(const Cell2D & target)
{
    mTarget = target;
}

inline bool MiniUnitsGroup::HasPathSet() const { return !mPath.empty(); }

inline const Cell2D & MiniUnitsGroup::GetPathTarget() const { return mTarget; }

} // namespace game
