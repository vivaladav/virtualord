#include "ObjectData.h"

#include "GameObjects/GameObject.h"

namespace game
{

const char * ObjectData::STR_CLASS[NUM_OBJ_CLASSES] =
{
    "BARRACKS",
    "BASE",
    "GENERATOR",
    "HOSPITAL",
    "RADAR",
    "RELICS",
    "RESEARCH",
    "STORAGE",
    "TARGET",
    "TOWER",
    "WALL",
    "GATE",

    "GENERIC",
    "MEDIC",
    "SOLDIER",
    "WORKER",

    "COLLECTABLE",
    "SCENE"
};

const char * ObjectData::STR_STAT[NUM_OBJ_ATTRIBUTES] =
{
    "ENERGY",
    "VIEW RADIUS",
    "FIRE POWER",
    "FIRE ACCURACY",
    "FIRE RANGE",
    "REGENERATION",
    "RESISTANCE",
    "SHIELD",
    "SPEED",
    "CONSTRUCTION",
    "CONQUEST",
    "HEALING",
};

const ObjectData ObjectData::NullObj({}, {}, {}, nullptr, GameObject::TYPE_NULL,
                                     OC_NULL, OCAT_UNDEFINED, 0, 0);

} // namespace game
