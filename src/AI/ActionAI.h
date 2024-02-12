#pragma once

#include "Cell2D.h"
#include "GameObjects/GameObjectTypes.h"

namespace game
{

class GameObject;

enum AIActionType : unsigned int
{
    AIA_NOP,

    AIA_END_TURN,
    AIA_NEW_UNIT,
    AIA_UNIT_ATTACK_ENEMY_UNIT,
    AIA_UNIT_COLLECT_BLOBS,
    AIA_UNIT_COLLECT_DIAMONDS,
    AIA_UNIT_COLLECT_LOOTBOX,
    AIA_UNIT_CONNECT_STRUCTURE,
    AIA_UNIT_CONQUER_GEN,

    NUM_AI_ACTIONS
};

// ===== ACTION AI =====
struct ActionAI
{
    unsigned int actId = 0;
    GameObject * ObjSrc = nullptr;
    GameObject * ObjDst = nullptr;
    Cell2D cellSrc = { -1, -1};
    Cell2D cellDst = { -1, -1};
    AIActionType type = AIA_NOP;
    int priority = 0;

    const char * GetTypeStr() const;
};

struct ActionAiComp
{
    bool operator()(const ActionAI * a, const ActionAI * b)
    {
        return a->priority < b->priority;
    }
};

// ===== ACTION AI NEW UNIT =====

struct ActionAINewUnit : public ActionAI
{
    GameObjectTypeId unitType;
};

} // namespace game
