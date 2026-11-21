#include "GameConstants.h"

namespace game
{

const LanguageId GameConstants::LANG_ENGLISH = 7183631951350908568u;
const LanguageId GameConstants::LANG_FRENCH = 144097093972160778u;
const LanguageId GameConstants::LANG_GERMAN = 9354183005477290313u;
const LanguageId GameConstants::LANG_ITALIAN = 9330455153790005779u;
const LanguageId GameConstants::LANG_PORTUGUES_BR = 3843580786482545913u;
const LanguageId GameConstants::LANG_SPANISH = 3685199145756854046u;

const LanguageId GameConstants::LANG_NULL = 0;

const char * FACTIONS_NAME[NUM_FACTIONS] =
{
    "Zulox",
    "Domens",
    "Alphen"
};

const char * MISSION_CATEGORY_TITLE[NUM_MISSION_CATEGORIES] =
{
    "RES_COLLECTION",
    "CONQUEST",
    "CREATION",
    "DESTRUCTION",
    "RES_PRODUCTION",
    "RESCUE",
    "RESISTANCE",
    "TUTORIAL"
};

} // namespace game
