#pragma once

#include "Screen.h"

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class Label; }
}

namespace game
{

class PanelResources;
class PanelPlanetActionConquer;
class PanelPlanetActionConquerAI;
class PanelPlanetActionExplore;
class PanelPlanetActions;
class PanelPlanetInfo;
class PanelPlanetResources;
class PlanetMap;
class TutorialManager;

class ScreenPlanetMap : public Screen
{
public:
    ScreenPlanetMap(Game * game);
    ~ScreenPlanetMap();

    void Update(float delta) override;
    void Render() override;

private:
    void SetPlanetName(const char * name);
    void SetDate(const char * date);

    void ShowInfo(int territory);

    bool TryToConquerTerritory(int index);

    void UpdatePlanetButtons();

private:
    sgl::graphic::Image * mBg = nullptr;

    PanelResources * mPanelPlayerRes = nullptr;
    PanelPlanetInfo * mPanelInfo = nullptr;
    PanelPlanetResources * mPanelResources = nullptr;
    PanelPlanetActions * mPanelActions = nullptr;
    PanelPlanetActionExplore * mPanelExplore = nullptr;
    PanelPlanetActionConquer * mPanelConquer = nullptr;
    PanelPlanetActionConquerAI * mPanelConquerAI = nullptr;

    sgl::sgui::Label * mLabelName = nullptr;
    sgl::sgui::Label * mLabelDate = nullptr;

    PlanetMap * mPlanet = nullptr;

    // TUTORIAL
    TutorialManager * mTutMan = nullptr;
};

} // namespace game
