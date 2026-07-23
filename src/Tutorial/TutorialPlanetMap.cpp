#include "Tutorial/TutorialPlanetMap.h"

#include "Screens/ScreenPlanetMap.h"

#include <cassert>

namespace game
{

TutorialPlanetMap::TutorialPlanetMap(Screen * screen, TutorialId tutId)
    : Tutorial(tutId)
    , mScreen(dynamic_cast<ScreenPlanetMap *>(screen))
{
    assert(mScreen != nullptr);
}

PlanetMap * TutorialPlanetMap::GetPlanetMap() const
{
    return mScreen->mPlanetMap;
}

PanelPlanetInfo * TutorialPlanetMap::GetPanelInfo() const { return mScreen->mPanelInfo; }
PanelPlanetResources * TutorialPlanetMap::GetPanelResources() const { return mScreen->mPanelResources; }
PanelPlanetActions * TutorialPlanetMap::GetPanelActions() const { return mScreen->mPanelActions; }
PanelPlanetActionExplore * TutorialPlanetMap::GetPanelExplore() const { return mScreen->mPanelExplore; }
PanelPlanetActionConquer * TutorialPlanetMap::GetPanelConquer() const { return mScreen->mPanelConquer; }

} // namespace game
