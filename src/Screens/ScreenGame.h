#pragma once

#include "Cell2D.h"
#include "MissionGoal.h"
#include "Screen.h"
#include "GameObjects/GameObjectAction.h"
#include "GameObjects/GameObjectTypes.h"

#include <sgl/core/Point.h>

#include <chrono>
#include <functional>
#include <unordered_map>
#include <vector>

namespace sgl
{
    namespace ai { class Pathfinder; }
    namespace graphic
    {
        class ParticlesManager;
        class ParticlesUpdater;
    }
}

namespace game
{

class AttackRangeIndicator;
class CameraMapController;
class ConquestIndicator;
class GameHUD;
class GameMap;
class GameObject;
class HealingRangeIndicator;
class Hospital;
class IsoLayer;
class IsoMap;
class MiniMap;
class MoveIndicator;
class Player;
class PlayerAI;
class StructureIndicator;
class TutorialManager;
class Unit;
class WallIndicator;

enum MissionGoalType : unsigned int;
enum PlayerFaction : unsigned int;

enum ParticlesUpdaterId : unsigned int
{
    PU_DAMAGE,
    PU_HEALING,
    PU_LOOTBOX_PRIZE,
    PU_SINGLE_LASER
};

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

    void ClearObjectAction(GameObject * obj);
    void SetObjectActionCompleted(GameObject * obj);
    void SetObjectActionFailed(GameObject * obj);

    sgl::graphic::ParticlesUpdater * GetParticleUpdater(ParticlesUpdaterId updaterId);

    void ClearSelection(Player * player);
    void SelectObject(GameObject * obj, Player * player);

    void CenterCameraOverCell(int row, int col);
    void CenterCameraOverObject(GameObject * obj);

    GameHUD * GetHUD() const;

    MiniMap * GetMiniMap() const;
    void SetMiniMapEnabled(bool val);

    bool GetPaused() const;
    void SetPause(bool paused);

    // MISSION GOALS
    const std::vector<MissionGoal> & GetMissionGoals() const;
    void CollectMissionGoalReward(unsigned int index);

    // TURN SYSTEM
    bool IsCurrentTurnLocal() const;

private:
    void OnApplicationQuit(sgl::core::ApplicationEvent & event) override;

    void InitMusic();
    void InitParticlesSystem();

    void CreateIsoMap();
    void CreateLayers();

    void CreateUI();

    void CreateTutorial();
    void UpdateTutorial(float delta);

    void LoadMapFile();

    void UpdateAI(float delta);
    void ExecuteAIAction(PlayerAI * ai);

    void CancelObjectAction(GameObject * obj);
    void SetObjectActionDone(GameObject * obj, bool successful);
    void FinalizeObjectAction(const GameObjectAction & action, bool successful);

    void UpdateGameEnd();
    bool CheckIfGoalCompleted(MissionGoal & g);
    void UpdateGoalCompletedIcon();
    void HandleGameOver();
    void HandleGameWon();
    void AssignMapToFaction(PlayerFaction faction);
    bool CheckGameOverForLocalPlayer();

    int CellToIndex(const Cell2D & cell) const;

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
    bool SetupUnitMove(Unit * unit, const Cell2D & start, const Cell2D & end,
                       const std::function<void(bool)> & onDone = [](bool){});
    bool SetupConnectCellsAI(Unit * unit, const std::function<void(bool)> & onDone = [](bool){});
    bool FindWhereToBuildStructureAI(Unit * unit, Cell2D & target);

    void HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitBuildStructureOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitBuildWallOnMouseUp(Unit * unit, const Cell2D & clickCell);

    void HandleSelectionClick(sgl::core::MouseButtonEvent & event);
    void HandleActionClick(sgl::core::MouseButtonEvent & event);

    bool StartUnitBuildWall(Unit * unit);

    void ShowActiveIndicators(Unit * unit, const Cell2D & cell);
    void ShowAttackIndicators(const GameObject * obj, int range);
    void ShowBuildStructureIndicator(Unit * unit, const Cell2D & currCell);
    void ShowBuildWallIndicator(Unit * unit, const Cell2D & dest);
    void ShowConquestIndicator(Unit * unit, const Cell2D & dest);
    void ShowHealingIndicators(const GameObject * obj, int range);
    void ShowMoveIndicator(GameObject * obj, const Cell2D & dest);
    void ClearCellOverlays();
    void ClearTempStructIndicator();

    void CenterCameraOverPlayerBase();

    void UpdateCurrentCell();

    // MISSION GOALS
    void SetMissionRewards();
    void TrackResourcesForGoals();
    void ClearResourcesTracking();

    // TURN
    void EndTurn();

    // SFX
    void PlayLocalActionErrorSFX(const Player * player);

private:
    friend class GameHUD;

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
    std::vector<unsigned int> mResourceTrackers;

    CameraMapController * mCamController = nullptr;

    unsigned int mIdOnSettingsChanged = 0;

    sgl::graphic::ParticlesManager * mPartMan = nullptr;

    std::chrono::time_point<std::chrono::steady_clock> mTimeStart;

    // UI
    GameHUD * mHUD = nullptr;

    GameMap * mGameMap = nullptr;
    IsoMap * mIsoMap = nullptr;

    sgl::ai::Pathfinder * mPathfinder = nullptr;

    MoveIndicator * mMoveInd = nullptr;

    struct Cell2D mCurrCell;
    sgl::core::Pointd2D mMousePos;

    // TUTORIAL
    TutorialManager * mTutMan = nullptr;

    // TURN MANAGEMENT
    Player * mLocalPlayer = nullptr;

    int mActivePlayerIdx = 0;

    float mTimerAutoEndTurn = 0.f;

    float mTimePlayed = 0.f;

    bool mMapCompleted = false;
    bool mPaused = false;
};

inline void ScreenGame::SetObjectActionCompleted(GameObject * obj)
{
    SetObjectActionDone(obj, true);
}
inline void ScreenGame::SetObjectActionFailed(GameObject * obj)
{
    SetObjectActionDone(obj, false);
}

inline GameHUD * ScreenGame::GetHUD() const { return mHUD; }

inline bool ScreenGame::GetPaused() const { return mPaused; }

inline const std::vector<MissionGoal> & ScreenGame::GetMissionGoals() const
{
    return mMissionGoals;
}

inline bool ScreenGame::IsCurrentTurnLocal() const { return mActivePlayerIdx == 0; }

} // namespace game
