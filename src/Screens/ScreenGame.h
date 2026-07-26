#pragma once

#include "Cell2D.h"
#include "Screen.h"
#include "GameObjects/GameObjectAction.h"
#include "GameObjects/GameObjectTypes.h"

#include <sgl/core/Point.h>

#include <functional>
#include <vector>

namespace sgl
{
    namespace ai { class Pathfinder; }
    namespace graphic { class ParticlesManager; }
    namespace utilities
    {
        class BinaryFile;
        class StringManager;
    }
}

namespace game
{

class CameraMapController;
class GameHUD;
class GameMap;
class GameObject;
class GameObjectsGroup;
class Hospital;
class IsoLayer;
class IsoMap;
class MiniMap;
class MiniUnit;
class MissionGoalsTracker;
class OverlayAttackRange;
class OverlayCellConquest;
class OverlayHealRange;
class OverlayPath;
class OverlaySelection;
class OverlayStructure;
class OverlayWall;
class Player;
class PlayerAI;
class Unit;

enum PlayerFaction : unsigned int;
enum TechUpgradeId : unsigned int;
enum TurnStage : unsigned int;

class ScreenGame : public Screen
{
public:
    ScreenGame(Game * game);
    ~ScreenGame();

    void InitNewGame();

    bool Load(sgl::utilities::BinaryFile & bf);
    bool Save(sgl::utilities::BinaryFile & bf) const override;

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

    void SetSelectionAllowed(bool allowed);
    bool IsSelectionAllowed() const;
    void ClearSelection(Player * player);
    void SelectObject(GameObject * obj, Player * player);

    // speed < 0 : instant move | speed == 0 : default speed | speed > 0 : use value
    void CenterCameraOverCell(int r, int c, float speed = -1.f);
    void CenterCameraOverCell(const Cell2D & cell, float speed = -1.f);
    void CenterCameraOverCell(unsigned int cellIndex, float speed = -1.f);
    void CenterCameraOverObject(const GameObject * obj, float speed = -1.f);
    void StopCameraMove();

    Player * GetActivePlayer() const;

    GameHUD * GetHUD() const;

    MiniMap * GetMiniMap() const;
    void SetMiniMapEnabled(bool val);

    MissionGoalsTracker * GetMissionGoalsTracker() const;

    bool GetPaused() const;
    void SetPause(bool paused);

    // TURN SYSTEM
    void SetLocalTurnStage(TurnStage ts);
    bool IsCurrentTurnLocal() const;

    bool CanLocalPlayerInteract() const;

private:
    void OnApplicationQuit(sgl::core::ApplicationEvent & event) override;

    void AssignStartResources(Player * p);

    void InitPlayers();
    void InitMusic();
    void InitParticlesSystem();
    void InitTutorial();

    void CreateIsoMap();
    void CreateLayers();

    void CreateUI();
    void CreateOverlays();
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
    void HandleGameOver();
    void HandleGameWon();
    void AssignMapToFaction(PlayerFaction faction);
    bool CheckGameOverForLocalPlayer();

    int CellToIndex(const Cell2D & cell) const;

    void ResetObjectAction(GameObject * obj);

    bool SetupNewMiniUnits(GameObjectTypeId type, GameObject * gen, GameObjectsGroup * group, Player * player,
                           int squads, int elements, const std::function<void(bool)> & onDone = [](bool){});
    bool SetupNewUnit(GameObjectTypeId type, GameObject * gen, Player * player,
                      const std::function<void(bool)> & onDone = [](bool){});
    bool SetupObjectInteraction(Unit * unit, GameObject * objTarget, Player * player,
                                const std::function<void(bool)> & onDone = [](bool){});
    bool SetupCellConquest(Unit * unit);
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

    void HandleUnitCellConquestOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitBuildStructureOnMouseUp(Unit * unit, const Cell2D & clickCell);
    void HandleUnitBuildWallOnMouseUp(Unit * unit, const Cell2D & clickCell);

    void HandleMiniUnitSetTargetOnMouseUp(GameObject * obj, const Cell2D & clickCell);

    void HandleSelectionClick(sgl::core::MouseButtonEvent & event);
    void HandleActionClick(sgl::core::MouseButtonEvent & event);

    bool StartUnitBuildWall(Unit * unit);

    void ShowActiveUnitIndicators(Unit * unit, const Cell2D & cell);
    void ShowActiveMiniUnitIndicators(MiniUnit * mu, const Cell2D & cell);
    void ShowBuildStructureIndicator(Unit * unit, const Cell2D & currCell);
    void ShowBuildWallIndicator(Unit * unit, const Cell2D & dest);
    void ShowCellConquestIndicator(Unit * unit, const Cell2D & dest);
    void ShowMoveIndicator(GameObject * obj, const Cell2D & dest);
    void ClearCellOverlays();

    int CheckBuildStructureValid(Unit * unit, const Cell2D & dest, bool building);

    void UpdatePanelHit(const GameObject * attacker);

    void UpdateCurrentCell();

    void AddObjectToMinimap(const Cell2D & cell, GameObjectTypeId type, PlayerFaction f);

    void OnUpgradeUnlocked(TechUpgradeId upgrade);

    // TURN
    void EndTurn();
    void InitLocalTurn();
    unsigned int GetNumLocalTurns() const;

    void ReselectLastSelected();

#ifdef DEV_MODE
    void CreateEnemyInCurrentCell(GameObjectTypeId type);
#endif

private:
    friend class GameHUD;
    friend class TutorialGame;

    std::vector<Player *> mAiPlayers;

    std::vector<GameObjectAction> mObjActions;
    std::vector<GameObjectAction> mObjActionsToDo;

    MissionGoalsTracker * mTrackerMG = nullptr;

    CameraMapController * mCamController = nullptr;

    unsigned int mIdOnSettingsChanged = 0;
    unsigned int mIdOnUnlockUpgraded = 0;

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
    OverlayAttackRange * mOverlayAttack = nullptr;
    OverlayCellConquest * mOverlayCellConquest = nullptr;
    OverlayHealRange * mOverlayHeal = nullptr;
    OverlayPath * mOverlayPath = nullptr;
    OverlaySelection * mOverlaySelection = nullptr;
    OverlayStructure * mOverlayStruct = nullptr;
    OverlayWall * mOverlayWall = nullptr;

    // TURN MANAGEMENT
    int mActivePlayerIdx = 0;
    Player * mLocalPlayer = nullptr;
    Player * mActiveplayer = nullptr;

    GameObject * mLastSelected = nullptr;

    TurnStage mTurnStage;

    float mTimerAutoEndTurn = 0.f;

    float mTimePlayed = 0.f;

    bool mPaused = false;
    bool mAllowSelection = true;
    bool mLocalTurnInitDone = false;
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

inline void ScreenGame::SetSelectionAllowed(bool allowed) { mAllowSelection = allowed; }
inline bool ScreenGame::IsSelectionAllowed() const { return mAllowSelection; }

inline Player * ScreenGame::GetActivePlayer() const { return mActiveplayer; }

inline GameHUD * ScreenGame::GetHUD() const { return mHUD; }

inline MissionGoalsTracker * ScreenGame::GetMissionGoalsTracker() const { return mTrackerMG; }

inline bool ScreenGame::GetPaused() const { return mPaused; }

inline bool ScreenGame::IsCurrentTurnLocal() const { return mActivePlayerIdx == 0; }

} // namespace game
