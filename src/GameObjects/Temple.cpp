#include "GameObjects/Temple.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "Screens/ScreenGame.h"
#include "Widgets/GameHUD.h"

#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/LoadedDie.h>
#include <sgl/utilities/UniformDistribution.h>

#include <vector>
#include <cmath>

namespace game
{

Temple::Temple()
    : Structure(GameObject::TYPE_TEMPLE, GameObject::CAT_GENERIC, 3, 3)
    , mOnDone([]{})
    , mExplorer(NO_FACTION)
{
    SetCanBeConquered(true);

    SetVisibilityLevel(4);

    DefineMaxValues();

    SetImage();
    SetObjColors();
}

void Temple::SetInvestedResources(int money, int material, int blobs, int diamonds)
{
    mInvestedMoney = money;
    mInvestedMaterial = material;
    mInvestedBlobs = blobs;
    mInvestedDiamonds = diamonds;

    if(mInvestedMoney > mMaxMoney)
        mInvestedMoney = mMaxMoney;

    if(mInvestedMaterial > mMaxMaterial)
        mInvestedMaterial = mMaxMaterial;

    if(mInvestedBlobs > mMaxBlobs)
        mInvestedBlobs = mMaxBlobs;

    if(mInvestedDiamonds > mMaxDiamonds)
        mInvestedDiamonds = mMaxDiamonds;

    const int maxTurns = 16;
    const int minTurns = 1;
    const float maxSuccess = 100.f;
    const float minSuccess = 0.f;

    // TIME
    const float timeInfluenceMoney = 35.f;
    const float timeInfluenceMaterial = 25.f;
    const float timeInfluenceBlobs = 20.f;
    const float timeInfluenceDiamonds = 20.f;

    const float timeCostMoney = timeInfluenceMoney / mMaxMoney;
    const float timeCostMaterial = timeInfluenceMaterial / mMaxMaterial;
    const float timeCostBlobs = timeInfluenceBlobs / mMaxBlobs;
    const float timeCostDiamonds = timeInfluenceDiamonds / mMaxDiamonds;

    const float timePerc = timeCostMoney * mInvestedMoney + timeCostMaterial * mInvestedMaterial +
                           timeCostBlobs * mInvestedBlobs + timeCostDiamonds * mInvestedDiamonds;
    mExplorationTurns = (maxTurns - static_cast<int>(maxTurns * timePerc / 100.f));

    if(mExplorationTurns < minTurns)
        mExplorationTurns = minTurns;

    // SUCCESS
    const float successInfluenceMoney = 20.f;
    const float successInfluenceMaterial = 15.f;
    const float successInfluenceBlobs = 32.5f;
    const float successInfluenceDiamonds = 32.5f;

    const float successCostMoney = successInfluenceMoney / mMaxMoney;
    const float successCostMaterial = successInfluenceMaterial / mMaxMaterial;
    const float successCostBlobs = successInfluenceBlobs / mMaxBlobs;
    const float successCostDiamonds = successInfluenceDiamonds / mMaxDiamonds;

    const float successPerc = (successCostMoney * mInvestedMoney +
                               successCostMaterial * mInvestedMaterial +
                               successCostBlobs * mInvestedBlobs +
                               successCostDiamonds * mInvestedDiamonds) / 100.f;
    mExplorationSuccess = std::roundf(minSuccess + (maxSuccess - minSuccess) * successPerc);

    if(mExplorationSuccess > maxSuccess)
        mExplorationSuccess = maxSuccess;
}

void Temple::StartExploring(PlayerFaction explorer, const std::function<void()> & onDone)
{
    // CREATE PROGRESS BAR

    // START
    mExplorer = explorer;

    mOnDone = onDone;
}

const char * Temple::GetExplorationOutcomeString(ExplorationOutcome o) const
{
    if(o >= NUM_EXPLORATION_OUTCOMES)
        return nullptr;

    static const char * strings[NUM_EXPLORATION_OUTCOMES] =
    {
        // -- REWARDS --
        // INCREASE EXISTING RESOURCES
        "Your money will be increased 10 times.",
        "Your energy and material will be maximized.",
        "Your blobs and diamonds will be maximized.",
        "All your resources will be maximized.",
        // INCREASE PRODUCTION
        "The production of all your current energy generators will double.",
        "The production of all your current material generators will double.",
        // MAXIMIZE COLLECTIBLES
        "All existing blobs on the map will be maximized.",
        "All existing diamonds on the map will be maximized.",

        // -- PUNISHMENTS --
        // DECREASE EXISTING RESOURCES
        "Your money will go down to 0.",
        "Your energy and material will go down to 0.",
        "Your blobs and diamonds will go down to 0.",
        "All your resources will go down to 0.",
        // DECREASE PRODUCTION
        "The production of all your current energy generators will halve.",
        "The production of all your current material generators will halve.",
        // MINIMIZE COLLECTIBLES
        "All existing blobs on the map will be minimized.",
        "All existing diamonds on the map will be minimized.",
    };

    return strings[o];
}

void Temple::OnNewTurn(PlayerFaction faction)
{
    GameObject::OnNewTurn(faction);

    // nothing to do while not exploring
    if(faction != mExplorer)
        return ;

    --mExplorationTurns;

    // still exploring...
    if(mExplorationTurns > 0)
        return ;

    // finalize exploration
    FinishExploring();

    mOnDone();

    // reset exploration
    mExplorer = NO_FACTION;

    mOnDone = []{};
}

void Temple::FinishExploring()
{
    const float probSuccess = mExplorationSuccess;
    const float probFail = 100.f - probSuccess;
    sgl::utilities::LoadedDie die({ EXP_OUTC_GOOD, EXP_OUTC_BAD }, { probSuccess, probFail });

    mOutcomeCat = static_cast<ExplorationOutcomeCategory>(die.GetNextValue());

    // GOOD -> positive reward
    if(mOutcomeCat == EXP_OUTC_GOOD)
    {
        DecideRewards();
        return ;
    }

    // BAD route -> decide if NOTHING or BAD
    sgl::utilities::LoadedDie die2({ EXP_OUTC_NOTHING, EXP_OUTC_BAD }, { probSuccess, probFail });

    mOutcomeCat = static_cast<ExplorationOutcomeCategory>(die2.GetNextValue());

    // NOTHING
    if(mOutcomeCat == EXP_OUTC_NOTHING)
    {
        mOutcome1 = EXP_OUT_NULL;
        mOutcome2 = EXP_OUT_NULL;
    }
    // BAD
    else
        DecidePunishments();
}

void Temple::DefineMaxValues()
{
    const int numLevels = 5;
    const int money[numLevels] = { 2000, 2500, 3500, 5000, 7000 };
    const int material[numLevels] = { 1000, 2000, 3000, 4000, 5000 };
    const int secRes[numLevels] = { 200, 250, 350, 500, 700 };

    const std::vector<float> lvlProbs = { 30.f, 25.f, 20.f, 15.f, 10.f };
    sgl::utilities::LoadedDie die(lvlProbs);
    const int lvl = die.GetNextValue();

    mMaxMoney = money[lvl];
    mMaxMaterial = material[lvl];
    mMaxBlobs = secRes[lvl];
    mMaxDiamonds = secRes[lvl];
}

void Temple::DecideRewards()
{
    sgl::utilities::UniformDistribution dist(FIRST_EXP_REW, LAST_EXP_REW);

    mOutcome1 = static_cast<ExplorationOutcome>(dist.GetNextValue());
    mOutcome2 = static_cast<ExplorationOutcome>(dist.GetNextValue());

    // same result -> try again
    if(mOutcome2 == mOutcome1)
        mOutcome2 = static_cast<ExplorationOutcome>(dist.GetNextValue());
    else
        return ;

    // same result again -> pick next
    if(mOutcome2 == mOutcome1)
        mOutcome2 = static_cast<ExplorationOutcome>(FIRST_EXP_REW + ((mOutcome2 + 1) % (LAST_EXP_REW + 1)));
}

void Temple::DecidePunishments()
{
    sgl::utilities::UniformDistribution dist(FIRST_EXP_PUN, LAST_EXP_PUN);

    mOutcome1 = static_cast<ExplorationOutcome>(dist.GetNextValue());
    mOutcome2 = static_cast<ExplorationOutcome>(dist.GetNextValue());

    // same result -> try again
    if(mOutcome2 == mOutcome1)
        mOutcome2 = static_cast<ExplorationOutcome>(dist.GetNextValue());
    else
        return ;

    // same result again -> pick next
    if(mOutcome2 == mOutcome1)
        mOutcome2 = static_cast<ExplorationOutcome>(FIRST_EXP_PUN + ((mOutcome2 + 1) % (LAST_EXP_PUN + 1)));
}

void Temple::UpdateGraphics()
{
    SetImage();

    SetObjColors();
}

void Temple::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_TEMPLE;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_TEMPLE + sel;
    else
        texInd = ID_STRUCT_TEMPLE_F1 + (faction * NUM_TEMPLE_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

void Temple::SetObjColors()
{
    mObjColors.clear();

    mObjColors.push_back(0xbaa35eff);
    mObjColors.push_back(0xa18245ff);
    mObjColors.push_back(0x7d6535ff);
    mObjColors.push_back(0xa07c45ff);
    mObjColors.push_back(0x8f743dff);
    mObjColors.push_back(0x745e32ff);
}

} // namespace game
