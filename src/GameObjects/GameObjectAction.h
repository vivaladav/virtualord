#pragma once

#include "Cell2D.h"

#include <functional>

namespace game
{

class GameMapProgressBar;
class GameObject;

enum GameObjectActionType : unsigned int;

struct GameObjectAction
{
    GameObjectAction(GameObject * go, GameObjectActionType aid, const std::function<void(bool)> & cb);
    GameObjectAction(GameObject * go, GameObjectActionType aid, const Cell2D & cell,
                     GameMapProgressBar * pb, const std::function<void(bool)> & cb);
    GameObjectAction(GameObject * go, GameObject * t, GameObjectActionType aid, const Cell2D & cell,
                     GameMapProgressBar * pb, const std::function<void(bool)> & cb);

    GameObject * obj = nullptr;
    GameObject * target = nullptr;

    GameMapProgressBar * progressBar = nullptr;

    Cell2D actionCell;

    GameObjectActionType type;

    // TRUE -> success / FALSE -> fail
    std::function<void(bool)> onDone = [](bool){};
};

inline GameObjectAction::GameObjectAction(GameObject * go, GameObjectActionType aid,
                                          const std::function<void(bool)> & cb)
    : obj(go)
    , type(aid)
    , onDone(cb)
{
}

inline GameObjectAction::GameObjectAction(GameObject * go, GameObjectActionType aid,
                                          const Cell2D & cell, GameMapProgressBar * pb,
                                          const std::function<void(bool)> & cb)
    : obj(go)
    , progressBar(pb)
    , actionCell(cell)
    , type(aid)
    , onDone(cb)
{
}

inline GameObjectAction::GameObjectAction(GameObject * go, GameObject * t, GameObjectActionType aid,
                                          const Cell2D & cell, GameMapProgressBar * pb,
                                          const std::function<void(bool)> & cb)
    : obj(go)
    , target(t)
    , progressBar(pb)
    , actionCell(cell)
    , type(aid)
    , onDone(cb)
{
}

} // namespace game
