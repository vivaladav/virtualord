#pragma once

#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringsChangeListener.h>

namespace sgl
{
    namespace sgui
    {
        class ButtonsGroup;
        class Image;
    }
}

namespace game
{

class ButtonMinimap;
class ButtonPanelSelectedObject;
class CameraMapController;
class CountdownLabel;
class DialogEndMission;
class DialogExit;
class DialogExploreTemple;
class DialogExploreTempleOutcome;
class DialogMissionGoals;
class DialogNewElement;
class DialogNewMiniUnitsSquad;
class DialogObject;
class DialogResearch;
class DialogTechTree;
class DialogTrading;
class DialogUpgrade;
class GameMapProgressBar;
class GameObject;
class MiniMap;
class MissionGoal;
class PanelHit;
class PanelObjectActions;
class PanelResources;
class PanelSelectedObject;
class PanelSelfDestruction;
class PanelShotType;
class PanelTurnControl;
class Player;
class ResearchCenter;
class ScreenGame;
class Temple;

struct Cell2D;

enum PlayerFaction : unsigned int;

// UI
class GameHUD : public sgl::sgui::Widget, public sgl::utilities::StringsChangeListener
{
public:
    GameHUD(ScreenGame * screen);
    ~GameHUD();

    bool IsShowingDialog() const;

    void SetMiniMapEnabled(bool val);
    MiniMap * GetMinimap() const;

    // OBJECT ACTIONS
    PanelObjectActions * GetPanelObjectActions() const;
    void HidePanelObjectActions();
    void ShowPanelObjectActions(GameObject * obj);
    void HidePanelSelfDestruction();
    void ShowPanelSelfDestruction();
    void HidePanelShotType();
    void ShowPanelShotType();

    void ShowPanelHit(const GameObject * attacker, const GameObject * target);
    void HidePanelHit();

    const sgl::sgui::ButtonsGroup * GetQuickUnitButtonsGroup() const;
    void SetQuickUnitButtonChecked(GameObject * obj);
    void ClearQuickUnitButtonChecked();

    DialogMissionGoals * GetDialogMissionGoals() const;
    void ShowDialogMissionGoals();
    void HideDialogMissionGoals();

    void ShowDialogEndMission(bool won);
    void ShowDialogExit();
    void ShowDialogExploreTemple(Player * player, Temple * temple);
    void HideDialogExploreTemple();
    void ShowDialogNewElement(unsigned int type);
    void HideDialogNewElement();
    DialogNewElement * GetDialogNewElement();

    void ShowMissionCountdown(int secs);
    void HideMissionCountdown();

    void ShowGoalCompletedIcon();
    void HideGoalCompletedIcon();

    void HidePanelSelectedObject();
    void ShowPanelSelectedObject(GameObject * obj);
    const PanelSelectedObject * GetPanelSelectedObject() const;

    void ShowTurnControlPanel();
    void ShowTurnControlText(const char * text);
    void UpdatePanelTurnControl();
    const PanelTurnControl * GetPanelTurnControl() const;

    void ShowDialogNewMiniUnitsSquad(GameObject * spawner);
    void HideDialogNewMiniUnitsSquad();

    void ShowDialogResearch(ResearchCenter * rc);
    void HideDialogResearch();

    void ShowDialogTechTree();
    void HideDialogTechTree();

    void ShowDialogTrading();
    void HideDialogTrading();

    void ShowDialogUpgrade(GameObject * obj);
    void HideDialogUpgrade();

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

    void TemporaryClosePanels();
    void ReopenPanels();

    void PositionOptionsPanelOverObjectActions(sgl::sgui::Widget * panel, unsigned int button);

    void ResumeGameFromExit();

    GameMapProgressBar * CreateProgressBar(float time, PlayerFaction faction);

    void CenterWidget(sgl::sgui::Widget * w);

    void OnStringsChanged() override;

private:
    PanelHit * mPanelHit = nullptr;
    PanelResources * mPanelRes = nullptr;
    PanelSelfDestruction * mPanelSelfDestruct = nullptr;
    PanelShotType * mPanelShotType = nullptr;
    PanelTurnControl * mPanelTurnCtrl = nullptr;
    ButtonMinimap * mButtonMinimap = nullptr;
    MiniMap * mMiniMap = nullptr;
    PanelObjectActions * mPanelObjActions = nullptr;
    sgl::sgui::ButtonsGroup * mGroupUnitSel = nullptr;
    CountdownLabel * mCountdownLabel = nullptr;

    DialogEndMission * mDialogEnd = nullptr;
    DialogExit * mDialogExit = nullptr;
    DialogExploreTemple * mDialogExploreTemple = nullptr;
    DialogExploreTempleOutcome * mDialogExploreTempleOutcome = nullptr;
    DialogMissionGoals * mDialogMissionGoals = nullptr;
    DialogNewElement * mDialogNewElement = nullptr;
    DialogNewMiniUnitsSquad * mDialogNewMiniUnits = nullptr;
    DialogObject * mDialogObj = nullptr;
    DialogResearch * mDialogResearch = nullptr;
    DialogTechTree * mDialogTechTree = nullptr;
    DialogTrading * mDialogTrading = nullptr;
    DialogUpgrade * mDialogUpgrade = nullptr;

    PanelSelectedObject * mPanelSelObj = nullptr;
    ButtonPanelSelectedObject * mButtonPanelSelObj = nullptr;

    sgl::sgui::Image * mGoalCompletedIcon = nullptr;

    ScreenGame * mScreen = nullptr;

    int mVisibleDialogs = 0;
};

inline bool GameHUD::IsShowingDialog() const { return mVisibleDialogs > 0; }

inline MiniMap * GameHUD::GetMinimap() const { return mMiniMap; }

inline const sgl::sgui::ButtonsGroup * GameHUD::GetQuickUnitButtonsGroup() const
{
    return mGroupUnitSel;
}

inline DialogMissionGoals * GameHUD::GetDialogMissionGoals() const { return mDialogMissionGoals; }

inline DialogNewElement * GameHUD::GetDialogNewElement() { return mDialogNewElement; }

inline PanelObjectActions * GameHUD::GetPanelObjectActions() const { return mPanelObjActions; }

inline const PanelSelectedObject * GameHUD::GetPanelSelectedObject() const { return mPanelSelObj; }

inline const PanelTurnControl * GameHUD::GetPanelTurnControl() const { return mPanelTurnCtrl; }
} // namespace game
