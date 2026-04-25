#pragma once

#include "Cell2D.h"
#include "GameObjects/GameObjectTypes.h"

#include <vector>

namespace game
{

class GameObject;

enum AIActionType : unsigned int
{
    AIA_NOP,

    AIA_END_TURN,
    AIA_IDLE_TURN,
    AIA_NEW_UNIT,
    AIA_UNIT_ATTACK_ENEMY_UNIT,
    AIA_UNIT_ATTACK_TREES,
    AIA_UNIT_BUILD_STRUCTURE,
    AIA_UNIT_BUILD_TOWER,
    AIA_UNIT_COLLECT_BLOBS,
    AIA_UNIT_COLLECT_DIAMONDS,
    AIA_UNIT_CONNECT_STRUCTURE,
    AIA_UNIT_CONQUER_STRUCT,
    AIA_UNIT_OPEN_LOOTBOX,
    AIA_UNIT_PATROL,
    AIA_UPGRADE_STRUCTURE,
    AIA_UPGRADE_UNIT,

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

    virtual ~ActionAI() {};

    const char * GetTypeStr() const;
};

struct ActionAiComp
{
    bool operator()(const ActionAI * a, const ActionAI * b)
    {
        return a->priority < b->priority;
    }
};

// ===== ACTION AI BUILD STRUCTURE =====

struct ActionAIBuildStructure : public ActionAI
{
    GameObjectTypeId structType;
};

// ===== ACTION AI NEW UNIT =====

struct ActionAINewUnit : public ActionAI
{
    GameObjectTypeId unitType;
};

// ===== ACTION AI NEW UNIT =====

struct ActionAIUpgradeObject : public ActionAI
{
    std::vector<int> attChanges;
};

// ===== ACTION AI IDLE TURN =====

struct ActionAIIdleTurn : public ActionAI
{
    float time;
};

} // namespace game
