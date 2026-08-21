#include "Tutorial/StepGameDialogTradingBuy.h"

#include "Widgets/DialogTrading.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Slider.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace
{
constexpr unsigned int valEnergy = 150;
constexpr unsigned int valMaterial = 100;
constexpr unsigned int valResearch = 130;
}

namespace game
{

StepGameDialogTradingBuy::StepGameDialogTradingBuy(GameHUD * HUD)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
    , mDialog(HUD->GetDialogTrading())
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(650, 100);

    const std::string strEnergy = sm->GetParametricString("TUT_GAME_TRAD_POST_4a",
                                                          std::to_string(valEnergy));
    info->AddActionEntry(strEnergy.c_str(), 0.f, false, true,
                        [this, info]
                        {
                            HandleButton(DialogTrading::TR_ENERGY, valEnergy);
                        });

    const std::string strMaterial = sm->GetParametricString("TUT_GAME_TRAD_POST_4b",
                                                            std::to_string(valMaterial));
    info->AddActionEntry(strMaterial.c_str(), 0.f, false, true,
                         [this, info]
                         {
                            HandleButton(DialogTrading::TR_MATERIAL, 100);
                         });

    const std::string strResearch = sm->GetParametricString("TUT_GAME_TRAD_POST_4c",
                                                            std::to_string(valResearch));
    info->AddActionEntry(strResearch.c_str(), 0.f, false, true,
                         [this, info]
                         {
                            HandleButton(DialogTrading::TR_RESEARCH, 130);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_TRAD_POST_3"), 0.f, false, true,
                         [this, info]
                         {
                            auto btn = mDialog->mButtonBuy;
                            btn->AddOnClickFunction([this]
                            {
                                GetPanelInfo()->Continue();
                            });

                            // FOCUS
                            const int padding = 10;
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH, true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const int fX2 = btn->GetScreenX();
                            const int fY2 = btn->GetScreenY();
                            const int fW2 = btn->GetWidth();
                            const int fH2 = btn->GetHeight();
                            GetClickFilter()->SetScreenClickableArea(fX2, fY2, fW2, fH2);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false,
                         [this]
                         {
                            auto btn = mDialog->mButtonClose;

                             // NOTE no need to remove the function later as the dialog is
                             // destroyed when the button is clicked
                             btn->AddOnClickFunction([this]
                                                    {
                                                        SetDone();
                                                    });

                             const int padding = 10;
                             const int x = btn->GetScreenX() - padding;
                             const int y = btn->GetScreenY() - padding;
                             const int w = btn->GetWidth() + (2 * padding);
                             const int h = btn->GetHeight() + (2 * padding);

                             GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                             mFocusArea->SetScreenArea(x, y, w, h, true);
                             mFocusArea->SetCornersColorAction();
                             mFocusArea->SetBlinking(true);
                             mFocusArea->SetVisible(true);
                         });
}

StepGameDialogTradingBuy::~StepGameDialogTradingBuy()
{
    delete mFocusArea;
}

void StepGameDialogTradingBuy::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

void StepGameDialogTradingBuy::HandleButton(unsigned int res, unsigned int value)
{
    auto btn = mDialog->mButtonsBuyPlus[res];
    btn->AddOnClickFunction([this, res, value]
                            {
                                if(mDialog->mBuy[res] == value)
                                {
                                    GetPanelInfo()->Continue();
                                }
                            });

    // FOCUS
    const int padding = 10;
    const int fX = btn->GetScreenX() - padding;
    const int fY = btn->GetScreenY() - padding;
    const int fW = btn->GetWidth() + (padding * 2);
    const int fH = btn->GetHeight() + (padding * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH, true);
    mFocusArea->SetVisible(true);

    // CLICK FILTER
    const int fX2 = btn->GetScreenX();
    const int fY2 = btn->GetScreenY();
    const int fW2 = btn->GetWidth();
    const int fH2 = btn->GetHeight();
    GetClickFilter()->SetScreenClickableArea(fX2, fY2, fW2, fH2);
}

} // namespace game
