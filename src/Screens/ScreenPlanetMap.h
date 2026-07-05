#pragma once

#include "Screen.h"

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class Label; }
}

namespace game
{

class DialogExit;
class PanelResources;
class PanelPlanetActionConquer;
class PanelPlanetActionConquerAI;
class PanelPlanetActionExplore;
class PanelPlanetActions;
class PanelPlanetInfo;
class PanelPlanetResources;
class PlanetMap;

class ScreenPlanetMap : public Screen
{
public:
    ScreenPlanetMap(Game * game);
    ~ScreenPlanetMap();

    void OnKeyUp(sgl::core::KeyboardEvent & event) override;

    void Update(float delta) override;
    void Render() override;

    void SetPause(bool paused);

private:
    void SetPlanetName(const char * name);
    void SetDate(const char * date);

    void ShowInfo(int territory);

    bool TryToConquerTerritory(int index);

    void UpdatePlanetButtons();

    void ShowDialogExit();

private:
    friend class TutorialPlanetMap;

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

    DialogExit * mDialogExit = nullptr;

    bool mPaused = false;
};

} // namespace game
