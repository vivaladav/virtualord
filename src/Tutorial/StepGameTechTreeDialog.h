#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <unordered_map>

namespace sgl
{
    namespace sgui { class AbstractButton; }
}

namespace game
{

class FocusArea;
class Game;
class GameHUD;

enum TechUpgradeId : unsigned int;

class StepGameTechTreeDialog : public TutorialInfoStep
{
public:
    StepGameTechTreeDialog(Game * game, GameHUD * HUD, TechUpgradeId upgradeID, bool showIntro);
    ~StepGameTechTreeDialog();

    void OnStart() override;
    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacks;

    sgl::sgui::AbstractButton * mBtn = nullptr;
    unsigned int mCallbackBtn = 0;

    Game *  mGame = nullptr;
    GameHUD * mHUD = nullptr;

    TechUpgradeId mUpgradeId;

    bool mWaitingPoints = false;
    int mWaitingPointsStep = 0;
};

} // namespace game
