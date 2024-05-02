#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace sgui { class ButtonsGroup; }
}

namespace game
{

class ButtonMinimap;
class ButtonPanelSelectedObject;
class CameraMapController;
class CountdownLabel;
class DialogExit;
class DialogExploreTemple;
class DialogExploreTempleOutcome;
class DialogNewElement;
class DialogObject;
class GameMapProgressBar;
class GameObject;
class MiniMap;
class PanelObjectActions;
class PanelResources;
class PanelSelectedObject;
class PanelTurnControl;
class Player;
class ScreenGame;
class Temple;
struct Cell2D;

enum PlayerFaction : unsigned int;

// UI
class GameHUD : public sgl::sgui::Widget
{
public:
    GameHUD(ScreenGame * screen);
    ~GameHUD();

    void SetMiniMapEnabled(bool val);
    MiniMap * GetMinimap() const;

    PanelObjectActions * GetPanelObjectActions() const;
    void HidePanelObjectActions();
    void ShowPanelObjectActions(GameObject * obj);

    const sgl::sgui::ButtonsGroup * GetQuickUnitButtonsGroup() const;
    void SetQuickUnitButtonChecked(GameObject * obj);
    void ClearQuickUnitButtonChecked();

    void ShowDialogEndMission(bool won);
    void ShowDialogExit();
    void ShowDialogExploreTemple(Player * player, Temple * temple);
    void HideDialogExploreTemple();
    void ShowDialogNewElement(unsigned int type);
    void HideDialogNewElement();
    DialogNewElement * GetDialogNewElement();

    void ShowMissionCountdown(int secs);
    void HideMissionCountdown();

    void HidePanelSelectedObject();
    void ShowPanelSelectedObject(GameObject * obj);
    const PanelSelectedObject * GetPanelSelectedObject() const;

    void ShowTurnControlPanel();
    void ShowTurnControlTextEnemyTurn();
    void ShowTurnControlTextGamePaused();
    const PanelTurnControl * GetPanelTurnControl() const;

    void SetLocalActionsEnabled(bool enabled);

    GameMapProgressBar * CreateProgressBarInCell(const Cell2D & cell, float time, PlayerFaction faction);

private:
    void HideDialogExploreTempleOutcome();
    void ShowDialogExploreTempleOutcome(Player * player, Temple * temple);
    void HideDialogObject();
    void ShowDialogObject(GameObject * obj);

    void ClosePanelMinimap();
    void OpenPanelMinimap();
    void ClosePanelSelectedObject();
    void OpenPanelSelectedObject();

    void TemporaryCloseSidePanels();
    void ReopenSidePanels();

    GameMapProgressBar * CreateProgressBar(float time, PlayerFaction faction);

    void CenterWidget(sgl::sgui::Widget * w);

private:
    PanelResources * mPanelRes = nullptr;
    PanelTurnControl * mPanelTurnCtrl = nullptr;
    ButtonMinimap * mButtonMinimap = nullptr;
    MiniMap * mMiniMap = nullptr;
    PanelObjectActions * mPanelObjActions = nullptr;
    sgl::sgui::ButtonsGroup * mGroupUnitSel = nullptr;
    CountdownLabel * mCountdownLabel = nullptr;

    DialogExit * mDialogExit = nullptr;
    DialogExploreTemple * mDialogExploreTemple = nullptr;
    DialogExploreTempleOutcome * mDialogExploreTempleOutcome = nullptr;
    DialogNewElement * mDialogNewElement = nullptr;
    DialogObject * mDialogObj = nullptr;

    PanelSelectedObject * mPanelSelObj = nullptr;
    ButtonPanelSelectedObject * mButtonPanelSelObj = nullptr;

    ScreenGame * mScreen = nullptr;
};

inline MiniMap * GameHUD::GetMinimap() const { return mMiniMap; }

inline DialogNewElement * GameHUD::GetDialogNewElement() { return mDialogNewElement; }

inline PanelObjectActions * GameHUD::GetPanelObjectActions() const { return mPanelObjActions; }

inline const PanelSelectedObject * GameHUD::GetPanelSelectedObject() const { return mPanelSelObj; }

inline const PanelTurnControl * GameHUD::GetPanelTurnControl() const { return mPanelTurnCtrl; }
} // namespace game
