#include "Screens/ScreenPlanetMap.h"

#include "Game.h"
#include "GameConstants.h"
#include "MapsRegistry.h"
#include "Player.h"
#include "States/StatesIds.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepPlanetMapConquerTerritory.h"
#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"
#include "Tutorial/StepPlanetMapIntro.h"
#include "Tutorial/StepPlanetMapExploreTerritory.h"
#include "Tutorial/StepPlanetMapExploreTerritoryInfo.h"
#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"
#include "Tutorial/StepPlanetMapNoInfo.h"
#include "Tutorial/StepPlanetMapSelectTerritory.h"
#include "Tutorial/StepPlanetMapSendAI.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/ButtonPlanetMap.h"
#include "Widgets/GameUIData.h"
#include "Widgets/PanelResources.h"
#include "Widgets/PanelPlanetActionConquer.h"
#include "Widgets/PanelPlanetActionConquerAI.h"
#include "Widgets/PanelPlanetActionExplore.h"
#include "Widgets/PanelPlanetActions.h"
#include "Widgets/PanelPlanetInfo.h"
#include "Widgets/PanelPlanetResources.h"
#include "Widgets/PlanetMap.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/LoadedDie.h>
#include <sgl/utilities/UniformDistribution.h>

namespace
{
    constexpr int costExploreMoney = 500;
    constexpr int costExploreEnergy = 300;
    constexpr int costExploreMaterial = 50;

    constexpr int costConquestMoney = 1000;
    constexpr int costConquestEnergy = 800;
    constexpr int costConquestMaterial = 200;
    constexpr int costConquestDiamonds = 10;
}

namespace game
{

ScreenPlanetMap::ScreenPlanetMap(Game * game)
    : Screen(game)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    graphic::Texture * tex = nullptr;
    graphic::Font * fnt = nullptr;

    const char * fileFont = "Lato-Regular.ttf";

    const unsigned int colorHeader = 0xe9f7fbcc;

    const int rendW = sgl::graphic::Renderer::Instance()->GetWidth();

    Player * player = game->GetLocalPlayer();

    // BACKGROUND
    tex = tm->GetTexture("space_bg.jpg");
    mBg = new graphic::Image(tex);

    // -- TOP PANELS --
    // PANEL PLAYER RESOURCES TOP BAR
    mPanelPlayerRes = new PanelResources(player, nullptr);
    mPanelPlayerRes->SetX((rendW - mPanelPlayerRes->GetWidth()) * 0.5f);

    // PANEL PLANET NAME
    tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_NAME);
    auto panelName = new sgui::Image(tex);

    fnt = fm->GetFont(fileFont, WidgetsConstants::FontSizePlanetMapTitle, graphic::Font::NORMAL);
    mLabelName = new sgui::Label(fnt, panelName);
    mLabelName->SetColor(colorHeader);

    // PANEL DATE
    tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_DATE);
    auto panelDate = new sgui::Image(tex);
    panelDate->SetX(mBg->GetWidth() - panelDate->GetWidth());

    fnt = fm->GetFont(fileFont, WidgetsConstants::FontSizePlanetMapTitle, graphic::Font::NORMAL);
    mLabelDate = new sgui::Label(fnt, panelDate);
    mLabelDate->SetColor(colorHeader);

    // -- PLANET --
    mPlanet = new PlanetMap;

    const int planetX = (mBg->GetWidth() - mPlanet->GetWidth()) * 0.5f;
    const int planetY = (mBg->GetHeight() - mPlanet->GetHeight()) * 0.5f;
    mPlanet->SetPosition(planetX, planetY);

    UpdatePlanetButtons();

    mPlanet->SetFunctionOnToggle([this](unsigned int ind, bool enabled)
                                 {
                                     if(!enabled)
                                         return;

                                     // enable panels
                                     mPanelActions->SetEnabled(true);
                                     mPanelInfo->SetEnabled(true);

                                     // make sure panel actions is visible
                                     mPanelActions->SetVisible(true);
                                     mPanelExplore->SetVisible(false);
                                     mPanelConquer->SetVisible(false);
                                     mPanelConquerAI->SetVisible(false);

                                     auto game = GetGame();
                                     const int planetId = game->GetCurrentPlanet();
                                     auto mapReg = game->GetMapsRegistry();

                                     const TerritoryStatus status = mapReg->GetMapStatus(planetId, ind);
                                     const PlayerFaction occupier = mapReg->GetMapOccupier(planetId, ind);
                                     const bool playerOccupier = game->GetLocalPlayerFaction() == occupier;
                                     mPanelActions->UpdateButtons(status, playerOccupier);

                                     if(status == TER_ST_FREE || status == TER_ST_OCCUPIED)
                                         ShowInfo(ind);
                                     else if(status == TER_ST_OCCUPIED_UNEXPLORED)
                                     {
                                         const int size = 0;
                                         const int value = 0;
                                         const MissionType mission = MISSION_UNKNOWN;

                                         mPanelInfo->SetEnabled(true);
                                         mPanelInfo->SetData(size, status, occupier, value, mission);
                                     }
                                     else
                                     {
                                         const int size = 0;
                                         const int value = 0;
                                         const MissionType mission = MISSION_UNKNOWN;

                                         mPanelResources->SetEnabled(false);

                                         mPanelInfo->SetData(size, status, occupier, value, mission);
                                     }

                                     mPanelExplore->ShowAction();
                                     mPanelConquerAI->ShowAction();
                                 });

    // -- MAIN PANELS --
    const int mainPanelsY0 = planetY;
    const int mainPanelsMarginH = 70;

    // PANEL RESOURCES
    mPanelResources = new PanelPlanetResources;
    mPanelResources->SetY(mainPanelsY0);
    mPanelResources->SetEnabled(false);

    // PANEL ACTIONS
    mPanelActions = new PanelPlanetActions;
    const int panActionsY = mPanelResources->GetY() + mPanelResources->GetHeight() + mainPanelsMarginH;
    mPanelActions->SetY(panActionsY);
    mPanelActions->SetEnabled(false);

    mPanelActions->AddOnButtonClickFunction(PanelPlanetActions::EXPLORE, [this]
    {
        mPanelActions->SetVisible(false);
        mPanelExplore->SetVisible(true);

        mPanelExplore->ShowAction();
    });

    mPanelActions->AddOnButtonClickFunction(PanelPlanetActions::CONQUER, [this]
    {
        mPanelActions->SetVisible(false);
        mPanelConquer->SetVisible(true);
    });

    mPanelActions->AddOnButtonClickFunction(PanelPlanetActions::SEND_AI, [this]
    {
        mPanelActions->SetVisible(false);
        mPanelConquerAI->SetVisible(true);

        mPanelConquerAI->ShowAction();
    });

    // PANEL ACTION EXPLORE
    mPanelExplore = new PanelPlanetActionExplore(player, costExploreMoney,
                                                 costExploreEnergy, costExploreMaterial);
    mPanelExplore->SetY(panActionsY);
    mPanelExplore->SetVisible(false);

    mPanelExplore->AddOnButtonOkClickFunction([this]
    {
        auto game = GetGame();
        Player * player = game->GetLocalPlayer();

        player->SumResource(Player::Stat::MONEY, -costExploreMoney);
        player->SumResource(Player::Stat::ENERGY, -costExploreEnergy);
        player->SumResource(Player::Stat::MATERIAL, -costExploreMaterial);

        // handle the result
        const int planetId = game->GetCurrentPlanet();
        const int territory = mPlanet->GetSelectedTerritoryId();
        auto mapReg = game->GetMapsRegistry();

        const PlayerFaction occupier = mapReg->GetMapOccupier(planetId, territory);

        if(occupier != NO_FACTION)
        {
            const float probFail = 90.f;
            const float probSuccess = 100.f - probFail;

            utilities::LoadedDie die({probFail, probSuccess});

            const bool res = die.GetNextValue();

            if(res)
            {
                mapReg->SetMapStatus(planetId, territory, TER_ST_OCCUPIED);

                ShowInfo(territory);
            }
            else
            {
                mapReg->SetMapStatus(planetId, territory, TER_ST_OCCUPIED_UNEXPLORED);

                // PANEL INFO
                const int size = 0;
                const int value = 0;
                const MissionType mission = MISSION_UNKNOWN;

                mPanelInfo->SetEnabled(true);
                mPanelInfo->SetData(size, TER_ST_OCCUPIED_UNEXPLORED, occupier, value, mission);
            }

            mPanelExplore->ShowResult(res);
        }
        else
        {
            mapReg->SetMapStatus(planetId, territory, TER_ST_FREE);

            mPanelExplore->ShowResult(true);

            ShowInfo(territory);
        }

        // update panel actions
        const TerritoryStatus status = mapReg->GetMapStatus(planetId, territory);
        const bool playerOccupier = game->GetLocalPlayerFaction() == occupier;

        mPanelActions->UpdateButtons(status, playerOccupier);

        mPlanet->SetButtonState(territory, occupier, status);
    });

    mPanelExplore->AddOnButtonCancelClickFunction([this]
    {
        mPanelActions->SetVisible(true);
        mPanelExplore->SetVisible(false);
    });

    // PANEL ACTION CONQUER
    mPanelConquer = new PanelPlanetActionConquer;
    mPanelConquer->SetY(panActionsY);
    mPanelConquer->SetVisible(false);

    mPanelConquer->AddOnButtonOkClickFunction([this]
    {
        Game * game = GetGame();

        const int territory = mPlanet->GetSelectedTerritoryId();
        game->SetCurrentTerritory(territory);
        game->RequestNextActiveState(StateId::GAME);
    });

    mPanelConquer->AddOnButtonCancelClickFunction([this]
    {
        mPanelActions->SetVisible(true);
        mPanelConquer->SetVisible(false);
    });

    // PANEL ACTION CONQUER AI
    mPanelConquerAI = new PanelPlanetActionConquerAI(player, costConquestMoney, costConquestEnergy,
                                                     costConquestMaterial, costConquestDiamonds);
    mPanelConquerAI->SetY(panActionsY);
    mPanelConquerAI->SetVisible(false);

    mPanelConquerAI->AddOnButtonOkClickFunction([this]
    {
        Game * game = GetGame();
        Player * player = game->GetLocalPlayer();

        // pay costs
        player->SumResource(Player::Stat::MONEY, -costConquestMoney);
        player->SumResource(Player::Stat::ENERGY, -costConquestEnergy);
        player->SumResource(Player::Stat::MATERIAL, -costConquestMaterial);
        player->SumResource(Player::Stat::DIAMONDS, -costConquestDiamonds);

        // attempt the conquest
        const int territory = mPlanet->GetSelectedTerritoryId();
        const bool res = TryToConquerTerritory(territory);

        const int planetId = game->GetCurrentPlanet();
        const PlayerFaction pf = game->GetLocalPlayerFaction();

        MapsRegistry * mapReg = game->GetMapsRegistry();

        // conquest successful
        if(res)
        {
            // update data
            const TerritoryStatus status = TER_ST_OCCUPIED;
            mapReg->SetMapStatus(planetId, territory, status);
            mapReg->SetMapOccupier(planetId, territory, pf);
            mapReg->SetMapMissionCompleted(planetId, territory);

            UpdatePlanetButtons();

            // update resources
            const int multMoney = 500;
            const int multRes1 = 100;
            const int multRes2 = 10;

            player->SumResource(Player::Stat::MONEY, multMoney * mapReg->GetMapValue(planetId, territory));
            player->SumResource(Player::Stat::ENERGY, multRes1 * mapReg->GetMapEnergy(planetId, territory));
            player->SumResource(Player::Stat::MATERIAL, multRes1 * mapReg->GetMapMaterial(planetId, territory));
            player->SumResource(Player::Stat::BLOBS, multRes2 * mapReg->GetMapBlobs(planetId, territory));
            player->SumResource(Player::Stat::DIAMONDS, multRes2 * mapReg->GetMapDiamonds(planetId, territory));

            // update UI
            mPlanet->SetButtonState(territory, pf, status);
            mPanelActions->UpdateButtons(status, true);

            ShowInfo(territory);
        }
        // conquest failed
        else
        {
            // decide winning fation
            PlayerFaction faction = mapReg->GetMapOccupier(planetId, territory);

            if(NO_FACTION == faction)
            {
                utilities::UniformDistribution dist(FACTION_1, FACTION_3);

                int f = dist.GetNextValue();

                if(f == pf)
                    f = ++f % NUM_FACTIONS;

                faction = static_cast<PlayerFaction>(f);

                mapReg->SetMapOccupier(planetId, territory, faction);
            }

            // update status
            const TerritoryStatus prevStatus = mapReg->GetMapStatus(planetId, territory);
            const TerritoryStatus status = prevStatus == TER_ST_FREE ?
                                           TER_ST_OCCUPIED : TER_ST_OCCUPIED_UNEXPLORED;

            mapReg->SetMapStatus(planetId, territory, status);

            // update UI
            mPlanet->SetButtonState(territory, faction, status);

            // PANEL INFO
            if(status == TER_ST_OCCUPIED_UNEXPLORED)
            {
                const int size = 0;
                const int value = 0;
                const MissionType mission = MISSION_UNKNOWN;

                mPanelInfo->SetEnabled(true);

                mPanelInfo->SetData(size, status, faction, value, mission);
            }
        }

        mPanelConquerAI->ShowResult(res);
    });

    mPanelConquerAI->AddOnButtonCancelClickFunction([this]
    {
        mPanelActions->SetVisible(true);
        mPanelConquerAI->SetVisible(false);
    });

    // PANEL INFO
    mPanelInfo = new PanelPlanetInfo;
    const int panelInfoX = mBg->GetWidth() - mPanelInfo->GetWidth();
    const int panelInfoY = mainPanelsY0;
    mPanelInfo->SetPosition(panelInfoX, panelInfoY);
    mPanelInfo->SetEnabled(false);

    // PANEL LEAVE
    tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_LEAVE);
    auto panelLeave = new sgui::Image(tex);
    panelLeave->SetPosition((mBg->GetWidth() - panelLeave->GetWidth()) / 2,
                            mBg->GetHeight() - panelLeave->GetHeight());

    auto buttonLeave = new ButtonLeavePlanet(panelLeave);
    buttonLeave->SetPosition((panelLeave->GetWidth() - buttonLeave->GetWidth()) / 2,
                              (panelLeave->GetHeight() - buttonLeave->GetHeight()) / 2);

    buttonLeave->AddOnClickFunction([this]
    {
        // TODO change to galaxy screen
        GetGame()->RequestNextActiveState(StateId::MAIN_MENU);
    });

    const int planetId = game->GetCurrentPlanet();
    SetPlanetName(PLANETS_NAME[planetId]);

    // TEST - REMOVE LATER
    SetDate("001 - 2200");

    // START MUSIC
    sgl::media::AudioManager::Instance()->GetPlayer()->PlayMusic("game/music_01.ogg");

    // TUTORIAL
    if(game->IsTutorialEnabled())
        CreateTutorial();
}

ScreenPlanetMap::~ScreenPlanetMap()
{
    sgl::sgui::Stage::Instance()->ClearWidgets();

    delete mBg;

    auto stage = sgl::sgui::Stage::Instance();

    stage->ClearWidgets();

    delete mTutMan;
}

void ScreenPlanetMap::Update(float delta)
{
    Game * game = GetGame();

    // TUTORIAL
    if(game->IsTutorialEnabled())
        mTutMan->Update(delta);
}

void ScreenPlanetMap::Render()
{
    mBg->Render();
}

void ScreenPlanetMap::CreateTutorial()
{
    Player * local = GetGame()->GetLocalPlayer();

    mTutMan = new TutorialManager;
    mTutMan->AddStep(new StepDelay(1.f));
    mTutMan->AddStep(new StepPlanetMapIntro);
    mTutMan->AddStep(new StepPlanetMapSelectTerritory(mPlanet));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepPlanetMapNoInfo(mPanelInfo, mPanelResources));
    mTutMan->AddStep(new StepPlanetMapExploreTerritory(mPanelActions));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepPlanetMapExploreTerritoryInfo);
    mTutMan->AddStep(new StepPlanetMapExploreTerritoryStart(mPanelExplore));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepPlanetMapExploreTerritorySuccess(mPanelExplore, mPanelInfo, mPanelResources));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepPlanetMapSendAI(mPanelActions));
    mTutMan->AddStep(new StepPlanetMapConquerTerritory(mPanelActions));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepPlanetMapConquerTerritoryStart(mPanelConquer));
    mTutMan->Start();
}

void ScreenPlanetMap::SetPlanetName(const char * name)
{
    mLabelName->SetText(name);

    sgl::sgui::Widget * parent = mLabelName->GetParent();
    const int x = (parent->GetWidth() - mLabelName->GetWidth()) * 0.5f;
    const int y = (parent->GetHeight() - mLabelName->GetHeight()) * 0.5f;
    mLabelName->SetPosition(x, y);
}

void ScreenPlanetMap::SetDate(const char * date)
{
    mLabelDate->SetText(date);

    sgl::sgui::Widget * parent = mLabelDate->GetParent();
    const int x = (parent->GetWidth() - mLabelDate->GetWidth()) * 0.5f;
    const int y = (parent->GetHeight() - mLabelDate->GetHeight()) * 0.5f;
    mLabelDate->SetPosition(x, y);
}

void ScreenPlanetMap::ShowInfo(int territory)
{
    auto game = GetGame();
    const int planetId = game->GetCurrentPlanet();
    auto mapReg = game->GetMapsRegistry();

    // PANEL RESOURCES
    mPanelResources->SetEnabled(true);
    mPanelResources->SetResourceValue(RES_ENERGY, mapReg->GetMapEnergy(planetId, territory));
    mPanelResources->SetResourceValue(RES_MATERIAL1, mapReg->GetMapMaterial(planetId, territory));
    mPanelResources->SetResourceValue(RES_BLOBS, mapReg->GetMapBlobs(planetId, territory));
    mPanelResources->SetResourceValue(RES_DIAMONDS, mapReg->GetMapDiamonds(planetId, territory));

    // PANEL INFO
    const int size = mapReg->GetMapSize(planetId, territory);
    const int value = mapReg->GetMapValue(planetId, territory);
    const PlayerFaction occupier = mapReg->GetMapOccupier(planetId, territory);
    const TerritoryStatus status = mapReg->GetMapStatus(planetId, territory);
    const MissionType mission = mapReg->GetMapMission(planetId, territory);

    mPanelInfo->SetEnabled(true);
    mPanelInfo->SetData(size, status, occupier, value, mission);
}

bool ScreenPlanetMap::TryToConquerTerritory(int index)
{
    auto game = GetGame();
    auto mapReg = game->GetMapsRegistry();

    const int planetId = game->GetCurrentPlanet();
    const TerritoryStatus status = mapReg->GetMapStatus(planetId, index);

    float probSuccess = 50.f;

    // bonus explored and free
    if(status == TER_ST_FREE)
    {
        const float bonusExploredAndFree = 30.f;
        probSuccess += bonusExploredAndFree;
    }
    // bonus explored
    else if(status == TER_ST_OCCUPIED)
    {
        const float bonusExplored = 15.f;
        probSuccess += bonusExplored;
    }

    const float probFail = 100.f - probSuccess;

    sgl::utilities::LoadedDie die({ probFail, probSuccess });
    return die.GetNextValue();;
}

void ScreenPlanetMap::UpdatePlanetButtons()
{
    Game * game = GetGame();

    const int planetId = game->GetCurrentPlanet();
    auto mapReg = game->GetMapsRegistry();

    const int numMaps = mapReg->GetNumMaps(planetId);

    for(int i = 0; i < numMaps; ++i)
    {
        const PlayerFaction occupier = mapReg->GetMapOccupier(planetId, i);
        const TerritoryStatus ts = mapReg->GetMapStatus(planetId, i);

        mPlanet->SetButtonState(i, occupier, ts);
    }
}

} // namespace game
