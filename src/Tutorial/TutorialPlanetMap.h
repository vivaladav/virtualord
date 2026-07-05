#pragma once

#include "Tutorial/Tutorial.h"

namespace game
{

class PanelPlanetInfo;
class PanelPlanetResources;
class PanelPlanetActions;
class PanelPlanetActionExplore;
class PanelPlanetActionConquer;
class PlanetMap;
class Screen;
class ScreenPlanetMap;

class TutorialPlanetMap : public Tutorial
{
public:
    TutorialPlanetMap(Screen * screen, TutorialId tutId);

protected:
    PlanetMap * GetPlanetMap() const;

    PanelPlanetInfo * GetPanelInfo() const;
    PanelPlanetResources * GetPanelResources() const;
    PanelPlanetActions * GetPanelActions() const;
    PanelPlanetActionExplore * GetPanelExplore() const;
    PanelPlanetActionConquer * GetPanelConquer() const;

private:
    ScreenPlanetMap * mScreen = nullptr;
};

} // namespace game
