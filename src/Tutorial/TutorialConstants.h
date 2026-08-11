#pragma once

namespace game
{

enum TutorialId : unsigned int
{
    TUTORIAL_MISSION_1,
    TUTORIAL_MISSION_2,
    TUTORIAL_MISSION_3,
    TUTORIAL_PLANET_MAP_1,
    TUTORIAL_PLANET_MAP_2,

    NUM_TUTORIALS,

    TUTORIAL_UNKNOWN
};

enum TutorialState : unsigned int
{
    TS_TODO,
    TS_IN_PROGRESS,
    TS_DONE,
    TS_ABORTED,

    NUM_TUTORIAL_STATES,

    TS_UNKNOWN
};

class TutorialConstants
{
public:
    static const unsigned int colorFocusAction;
    static const unsigned int colorFocusElement;

    static const unsigned int colorText;
    static const unsigned int colorTextAction;
    static const unsigned int colorTextContinue;
    static const unsigned int colorTextOld;

    static const int infoPlanetMapX;
    static const int infoPlanetMapY;
    static const int infoPlanetMapW;
    static const int infoPlanetMapH;

    // structure categories in build dialog
    static const int catGeneric;
    static const int catDefenses;
    static const int catResources;
    static const int catTech;

    // indexes of structures in build dialog
    static const int structBunker;
    static const int structGate;
    static const int structDefTower;
    static const int structMatExtr;
    static const int structResearchCenter;
    static const int structSolarPanel;
    static const int structTradingPost;
};

} // namespace game
