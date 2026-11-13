#include "Tutorial/StepGameTechTreeDialog.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectData.h"
#include "Widgets/ButtonTechUpgrade.h"
#include "Widgets/DialogTechTree.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace
{
constexpr int padding = 10;

constexpr int infoX0 = 700;
constexpr int infoY0 = 900;
}

namespace game
{

StepGameTechTreeDialog::StepGameTechTreeDialog(Game * game, GameHUD * HUD, TechUpgradeId upgradeID,
                                               bool showIntro)
    : TutorialInfoStep(700, 140)
    , mFocusArea(new FocusArea)
    , mGame(game)
    , mHUD(HUD)
    , mUpgradeId(upgradeID)
    , mWaitingPoints(!game->CanLocalPlayerAffordTechUpgrade(upgradeID))
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(infoX0, infoY0);

    if(showIntro && !mWaitingPoints)
        info->AddInfoEntry(sm->GetCString("TUT_GAME_UPGRADES_1"), 9.f, true, true);

    // not enough research points yet
    if(mWaitingPoints)
    {
        info->AddInfoEntry(sm->GetCString("TUT_GAME_UPGRADES_4"), 0.f, false, true,
                           [this, info, upgradeID]
                        {
                            info->SetPosition(900, 200);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mVisibleButtonsUpgrade.at(upgradeID);

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->ExpandClickableArea();

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetCornersColorElement();
                            mFocusArea->SetVisible(true);
                        });

        info->AddActionEntry(sm->GetCString("TUT_GAME_RES_CEN_1"), 0.f, false, true);

        info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_ICON"),
                         0.f, false, false, [this, game, HUD, info]
                        {
                            info->SetPosition(1000, 650);

                            auto panel = HUD->GetPanelObjectActions();

                            auto btn = panel->GetButton(PanelObjectActions::BTN_TECH_TREE);

                            // CLICK FILTER
                            const int fX = btn->GetScreenX();
                            const int fY = btn->GetScreenY();
                            const int fW = btn->GetWidth();
                            const int fH = btn->GetHeight();

                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonSelect());
                            cf->SetScreenClickableArea(fX, fY, fW, fH);

                            // FOCUS
                            const int padding = 10;
                            const int f2X = fX - padding;
                            const int f2Y = fY - padding;
                            const int f2W = fW + (padding * 2);
                            const int f2H = fH + (padding * 2);

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H, true);
                            mFocusArea->SetVisible(true);
                        });
    }

    // normal handling of upgrade unlocking
    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_2"), 0.f, false, true,
                         [this, info, upgradeID]
                        {
                            info->SetPosition(infoX0, infoY0);
                            info->SetFocus();

                            auto dialog = mHUD->GetDialogTechTree();
                            mBtn = dialog->mVisibleButtonsUpgrade.at(upgradeID);

                            mCallbackBtn = mBtn->AddOnToggleFunction([info](bool checked)
                                {
                                    if(checked)
                                        info->Continue();
                                });

                            const int x = mBtn->GetScreenX() - padding;
                            const int y = mBtn->GetScreenY() - padding;
                            const int w = mBtn->GetWidth() + (2 * padding);
                            const int h = mBtn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                            mFocusArea->SetCornersColorAction();

                            // move elements to front in case dialog was reopened
                            auto stage = sgl::sgui::Stage::Instance();
                            stage->MoveChildToFront(mFocusArea);
                            stage->MoveChildToFront(GetClickFilter());
                            stage->MoveChildToFront(GetPanelInfo());
                        });

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_3"), 0.f, false, true,
                         [this, info]
                         {
                            info->SetPosition(800, 900);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mBtnUnlock;

                            const auto cid = btn->AddOnClickFunction([info]
                                {
                                    info->Continue();
                                });

                            mCallbacks.emplace(btn, cid);

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false,
                         [this, info]
                        {
                            info->SetPosition(900, 20);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mBtnClose;

                            const auto cid = btn->AddOnClickFunction([this]
                                                    {
                                                        SetDone();
                                                    });

                            mCallbacks.emplace(btn, cid);

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });
}

StepGameTechTreeDialog::~StepGameTechTreeDialog()
{
    delete mFocusArea;

    // clear callbacks
    for(auto it : mCallbacks)
        (it.first)->RemoveClickFunction(it.second);

    if(mBtn != nullptr)
        mBtn->RemoveToggleFunction(mCallbackBtn);
}

void StepGameTechTreeDialog::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

void StepGameTechTreeDialog::Update(float delta)
{
    TutorialInfoStep::Update(delta);

    if(!mWaitingPoints)
        return ;

    if(mWaitingPointsStep == 0)
    {
        auto dialog = mHUD->GetDialogTechTree();

        mFocusArea->SetVisible(dialog != nullptr);

        if(mGame->CanLocalPlayerAffordTechUpgrade(mUpgradeId))
        {
            GetPanelInfo()->Continue();

            ++mWaitingPointsStep;
        }
    }
    else if(mWaitingPointsStep == 1)
    {
        auto obj = mGame->GetLocalPlayer()->GetSelectedObject();

        if(obj != nullptr && obj->GetObjectType() == ObjectData::TYPE_RESEARCH_CENTER)
        {
            GetPanelInfo()->Continue();

            ++mWaitingPointsStep;
        }
    }
    else if(mWaitingPointsStep == 2)
    {
        auto dialog = mHUD->GetDialogTechTree();

        if(dialog != nullptr)
        {
            GetPanelInfo()->Continue();

            ++mWaitingPointsStep;
            mWaitingPoints = false;
        }
    }
}

} // namespace game
