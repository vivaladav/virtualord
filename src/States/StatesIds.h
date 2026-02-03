#pragma once

namespace game
{

enum StateId : int
{
    INIT,
    INIT_GAME,
    GAME,
    MAIN_MENU,
    FACTION_SEL,
    NEW_GAME,
    PLANET_MAP,
    SETTINGS,
    TEST,

    NUM_SCREENS // keep last
};

} // namespace game
