#pragma once

#include "Structure.h"

#include <functional>

namespace game
{

class GameProgressBar;

class Temple : public Structure
{
public:
    enum ExplorationOutcomeCategory : unsigned int
    {
        EXP_OUTC_GOOD,
        EXP_OUTC_NOTHING,
        EXP_OUTC_BAD,

        NUM_EXPLORATION_OUTCOME_CATEGORIES,

        EXP_OUTC_NULL
    };

    enum ExplorationOutcome : unsigned int
    {
        // -- REWARDS --
        // INCREASE EXISTING RESOURCES
        EXP_REW_MULT10_MONEY,
        EXP_REW_MAX_RES_ENE_MAT,
        EXP_REW_MAX_RES_BLO_DIA,
        EXP_REW_MAX_RESOURCES,
        // INCREASE PRODUCTION
        EXP_REW_INC_ENERGY_PRODUCTION,
        EXP_REW_INC_MATERIAL_PRODUCTION,
        // MAXIMIZE COLLECTIBLES
        EXP_REW_MAX_BLOBS,
        EXP_REW_MAX_DIAMONDS,

        // -- PUNISHMENTS --
        // DECREASE EXISTING RESOURCES
        EXP_PUN_ZERO_MONEY,
        EXP_PUN_ZERO_RES_ENE_MAT,
        EXP_PUN_ZERO_RES_BLO_DIA,
        EXP_PUN_ZERO_RESOURCES,
        // DECREASE PRODUCTION
        EXP_PUN_DEC_ENERGY_PRODUCTION,
        EXP_PUN_DEC_MATERIAL_PRODUCTION,
        // MINIMIZE COLLECTIBLES
        EXP_PUN_MIN_BLOBS,
        EXP_PUN_MIN_DIAMONDS,

        NUM_EXPLORATION_OUTCOMES,

        // FIRST/LAST VALUES - update when adding new outcomes
        FIRST_EXP_REW = EXP_REW_MULT10_MONEY,
        LAST_EXP_REW = EXP_REW_MAX_DIAMONDS,
        NUM_EXP_REW = LAST_EXP_REW - FIRST_EXP_REW + 1,
        FIRST_EXP_PUN = EXP_PUN_ZERO_MONEY,
        LAST_EXP_PUN = EXP_PUN_MIN_DIAMONDS,
        NUM_EXP_PUN = LAST_EXP_PUN - FIRST_EXP_PUN + 1,

        // NULL
        EXP_OUT_NULL
    };

public:
    Temple();

    void SetInvestedResources(int money, int material, int blobs, int diamonds);
    int GetInvestedMoney() const;
    int GetInvestedMaterial() const;
    int GetInvestedBlobs() const;
    int GetInvestedDiamonds() const;

    int GetExplorationTurns() const;
    int GetExplorationSuccessRate() const;

    void StartExploring(PlayerFaction explorer, const std::function<void()> & onDone);
    ExplorationOutcomeCategory GetExplorationOutcomeCategory() const;
    ExplorationOutcome GetExplorationOutcome1() const;
    ExplorationOutcome GetExplorationOutcome2() const;
    const char * GetExplorationOutcomeString(ExplorationOutcome o) const;

    int GetMaxInvestableMoney() const;
    int GetMaxInvestableMaterial() const;
    int GetMaxInvestableBlobs() const;
    int GetMaxInvestableDiamonds() const;

    void OnNewTurn(PlayerFaction faction) override;

private:
    void FinishExploring();

    void DefineMaxValues();

    void DecideRewards();
    void DecidePunishments();

    void UpdateGraphics() override;

    void SetImage();
    void SetObjColors();

private:
    std::function<void()> mOnDone;

    int mMaxMoney = 0;
    int mMaxMaterial = 0;
    int mMaxBlobs = 0;
    int mMaxDiamonds = 0;

    int mInvestedMoney = 0;
    int mInvestedMaterial = 0;
    int mInvestedBlobs = 0;
    int mInvestedDiamonds = 0;

    int mExplorationTurns = 0;
    int mExplorationSuccess = 0;

    ExplorationOutcomeCategory mOutcomeCat = EXP_OUTC_NULL;
    ExplorationOutcome mOutcome1 = EXP_OUT_NULL;
    ExplorationOutcome mOutcome2 = EXP_OUT_NULL;

    PlayerFaction mExplorer;

    GameProgressBar * mProgressBar = nullptr;
};

inline int Temple::GetInvestedMoney() const { return mInvestedMoney; }
inline int Temple::GetInvestedMaterial() const { return mInvestedMaterial; }
inline int Temple::GetInvestedBlobs() const { return mInvestedBlobs; }
inline int Temple::GetInvestedDiamonds() const { return mInvestedDiamonds; }

inline int Temple::GetExplorationTurns() const { return mExplorationTurns; }
inline int Temple::GetExplorationSuccessRate() const { return mExplorationSuccess; }

inline Temple::ExplorationOutcomeCategory Temple::GetExplorationOutcomeCategory() const
{
    return mOutcomeCat;
}

inline Temple::ExplorationOutcome Temple::GetExplorationOutcome1() const { return mOutcome1; }
inline Temple::ExplorationOutcome Temple::GetExplorationOutcome2() const { return mOutcome2; }

inline int Temple::GetMaxInvestableMoney() const { return mMaxMoney; }
inline int Temple::GetMaxInvestableMaterial() const { return mMaxMaterial; }
inline int Temple::GetMaxInvestableBlobs() const { return mMaxBlobs; }
inline int Temple::GetMaxInvestableDiamonds() const { return mMaxDiamonds; }

} // namespace game
