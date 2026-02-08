#pragma once

#include "Cell2D.h"
#include "MissionGoal.h"
#include "Screen.h"
#include "GameObjects/GameObjectAction.h"
#include "GameObjects/GameObjectTypes.h"

#include <sgl/core/Point.h>

#include <functional>
#include <unordered_map>
#include <vector>

namespace sgl
{
    namespace ai { class Pathfinder; }
    namespace graphic { class ParticlesManager; }
    namespace utilities { class StringManager; }
}

namespace game
{

class AttackRangeIndicator;
class CameraMapController;
class ConquestIndicator;
class GameHUD;
class GameMap;
class GameObject;
class GameObjectsGroup;
class HealingRangeIndicator;
class Hospital;
class IsoLayer;
class IsoMap;
class MiniMap;
class MiniUnit;
class PathIndicator;
class PathOverlay;
class Player;
class PlayerAI;
class StructureIndicator;
class Unit;
class WallIndicator;

enum PlayerFaction : unsigned int;
enum TurnStage : unsigned int;

class ScreenGame : public Screen
{
public:
    ScreenGame(Game * game);
    ~ScreenGame();

    unsigned int GetPlayTimeInSec() const;

    void Update(float delta) override;
    void Render() override;

    void OnKeyDown(sgl::core::KeyboardEvent & event) override;
    void OnKeyUp(sgl::core::KeyboardEvent & event) override;
    void OnMouseButtonUp(sgl::core::MouseButtonEvent & event) override;
    void OnMouseMotion(sgl::core::MouseMotionEvent & event) override;
    void OnWindowExposed(sgl::graphic::WindowEvent & event) override;
    void OnWindowHidden(sgl::graphic::WindowEvent & event) override;
    void OnWindowMouseEntered(sgl::graphic::WindowEvent & event) override;
    void OnWindowMouseLeft(sgl::graphic::WindowEvent & event) override;

    void OnAutomaticMovesFinished();

    void OnObjectDestroyed(GameObject * obj);

    void SetObjectActionCompleted(GameObject * obj);
    void SetObjectActionFailed(GameObject * obj);

    const sgl::graphic::ParticlesManager * GetParticlesManager() const;

    void ClearSelection(Player * player);
    void SelectObject(GameObject * obj, Player * player);

    void CenterCameraOverObject(const GameObject * obj);

    Player * GetActivePlayer() const;

    GameHUD * GetHUD() const;

    MiniMap * GetMiniMap() const;
    void SetMiniMapEnabled(bool val);

    bool GetPaused() const;
    void SetPause(bool paused);

    // MISSION GOALS
    const std::vector<MissionGoal> & GetMissionGoals() const;
    void CollectMissionGoalReward(unsigned int index);

    // TURN SYSTEM
    void SetLocalTurnStage(TurnStage ts);
    bool IsCurrentTurnLocal() const;

    bool CanLocalPlayerInteract() const;

private:
    void OnApplicationQuit(sgl::core::ApplicationEvent & event) override;

    void InitMusic();
    void InitParticlesSystem();

    void CreateIsoMap();
    void CreateLayers();

    void CreateUI();
    void HideActionPanels();

    void LoadMapFile();

    void UpdateAI(float delta);
    void ExecuteAIAction(PlayerAI * ai);

    void ClearObjectAction(GameObject * obj);
    void CancelObjectAction(GameObject * obj);
    void SetObjectActionDone(GameObject * obj, bool successful);
    void FinalizeObjectAction(const GameObjectAction & action, bool successful);

    void CancelMiniUnitsGroupPath(GameObjectsGroup * group);

    void UpdateGameEnd();
    bool CheckIfGoalCompleted(MissionGoal & g);
    void UpdateGoalCompletedIcon();
    void HandleGameOver();
    void HandleGameWon();
    void AssignMapToFaction(PlayerFaction faction);
    bool CheckGameOverForLocalPlayer();

    int CellToIndex(const Cell2D & cell) const;

    bool SetupNewMiniUnits(GameObjectTypeId type, GameObject * gen, GameObjectsGroup * group, Player * player,
                           int squads, int elements, const std::function<void(bool)> & onDone = [](bool){});
    bool SetupNewUnit(GameObjectTypeId type, GameObject * gen, Player * player,
                      const std::function<void(bool)> & onDone = [](bool){});
    bool SetupStructureConquest(Unit * unit, const Cell2D & start, const Cell2D & end, Player * player,
                                const std::function<void(bool)> & onDone = [](bool){});
    bool SetupStructureBuilding(Unit * unit, const Cell2D & cellTarget, Player * player,
                                const std::function<void(bool)> & onDone = [](bool){});
    bool SetupUnitAttack(Unit * unit, GameObject * target, Player * player,
                         const std::function<void(bool)> & onDone = [](bool){});
    bool SetupHospitalHeal(Hospital * hospital, GameObject * target, Player * player,
                           const std::function<void(bool)> & onDone = [](bool){});
    bool SetupUnitHeal(Unit * unit, GameObject * target, Player * player,
                       const std::function<void(bool)> & onDone = [](bool){});
    bool SetupUnitMove(Unit * unit, const Cell2D & start, const Cell2D & end, bool ignoreEnergy,
                       const std::function<void(bool)> & onDone = [](bool){});
    bool SetupConnectCellsAI(Unit * unit, const std::function<void(bool)> & onDone = [](bool){});

    void HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitBuildStructureOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitBuildWallOnMouseUp(Unit * unit, const Cell2D & clickCell);

    void HandleMiniUnitSetTargetOnMouseUp(GameObject * obj, const Cell2D & clickCell);

    void HandleSelectionClick(sgl::core::MouseButtonEvent & event);
    void HandleActionClick(sgl::core::MouseButtonEvent & event);

    bool StartUnitBuildWall(Unit * unit);

    void ShowActiveUnitIndicators(Unit * unit, const Cell2D & cell);
    void ShowActiveMiniUnitIndicators(MiniUnit * mu, const Cell2D & cell);
    void ShowAttackIndicators(const GameObject * obj, int range);
    void ShowBuildStructureIndicator(Unit * unit, const Cell2D & currCell);
    void ShowBuildWallIndicator(Unit * unit, const Cell2D & dest);
    void ShowConquestIndicator(Unit * unit, const Cell2D & dest);
    void ShowHealingIndicators(const GameObject * obj, int range);
    void ShowMoveIndicator(GameObject * obj, const Cell2D & dest);
    void ClearCellOverlays();
    void ClearTempStructIndicator();

    void UpdatePanelHit(const GameObject * attacker);

    void UpdateCurrentCell();

    void AddObjectToMinimap(const Cell2D & cell, GameObjectTypeId type, PlayerFaction f);

    // MISSION GOALS
    void SetMissionRewards();
    void TrackResourcesForGoals();
    void ClearResourcesTracking();

    // TURN
    void EndTurn();
    void InitLocalTurn();

    void ReselectLastSelected();

#ifdef DEV_MODE
    void CreateEnemyInCurrentCell(GameObjectTypeId type);
#endif

private:
    friend class GameHUD;
    friend class TutorialGameIntro;

    std::vector<Player *> mAiPlayers;

    std::vector<ConquestIndicator *> mConquestIndicators;
    std::unordered_map<GameObjectTypeId, StructureIndicator *> mStructIndicators;
    std::vector<WallIndicator *> mWallIndicators;
    std::vector<AttackRangeIndicator *> mAttIndicators;
    std::vector<HealingRangeIndicator *> mHealIndicators;
    StructureIndicator * mTempStructIndicator = nullptr;

    std::vector<unsigned int> mConquestPath;
    std::vector<unsigned int> mWallPath;

    std::vector<GameObjectAction> mObjActions;
    std::vector<GameObjectAction> mObjActionsToDo;

    std::vector<MissionGoal> mMissionGoals;
    std::vector<int> mResourcesGained;
    std::vector<unsigned int> mResourceTrackerIds;

    CameraMapController * mCamController = nullptr;

    unsigned int mIdOnSettingsChanged = 0;

    sgl::graphic::ParticlesManager * mPartMan = nullptr;

    sgl::utilities::StringManager * mSM = nullptr;

    // UI
    GameHUD * mHUD = nullptr;

    GameMap * mGameMap = nullptr;
    IsoMap * mIsoMap = nullptr;

    sgl::ai::Pathfinder * mPathfinder = nullptr;

    struct Cell2D mCurrCell;
    sgl::core::Pointd2D mMousePos;

    // MAP OVERLAYS
    PathOverlay * mPathOverlay = nullptr;

    PathIndicator * mPathIndicator = nullptr;

    // TURN MANAGEMENT
    Player * mLocalPlayer = nullptr;

    GameObject * mLastSelected = nullptr;

    TurnStage mTurnStage;

    int mActivePlayerIdx = 0;

    float mTimerAutoEndTurn = 0.f;

    float mTimePlayed = 0.f;

    bool mMapCompleted = false;
    bool mPaused = false;
    bool mTutorialStarted = false;
};

inline void ScreenGame::SetObjectActionCompleted(GameObject * obj)
{
    SetObjectActionDone(obj, true);
}
inline void ScreenGame::SetObjectActionFailed(GameObject * obj)
{
    SetObjectActionDone(obj, false);
}

inline const sgl::graphic::ParticlesManager * ScreenGame::GetParticlesManager() const
{
    return mPartMan;
}

inline GameHUD * ScreenGame::GetHUD() const { return mHUD; }

inline bool ScreenGame::GetPaused() const { return mPaused; }

inline const std::vector<MissionGoal> & ScreenGame::GetMissionGoals() const
{
    return mMissionGoals;
}

inline bool ScreenGame::IsCurrentTurnLocal() const { return mActivePlayerIdx == 0; }

} // namespace game
