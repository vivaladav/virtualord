#pragma once

namespace game
{

enum TutorialId : unsigned int
{
    TUTORIAL_MISSION_INTRO,
    TUTORIAL_PLANET_MAP,

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
};

} // namespace game
