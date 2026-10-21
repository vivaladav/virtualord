#include "Widgets/DialogAutoAttackSettings.h"

#include "GameObjects/GameObject.h"
#include "GameObjectTools/Weapon.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameComboBox.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <string>

namespace game
{

DialogAutoAttackSettings::DialogAutoAttackSettings(GameObject * obj)
    : mObj(obj)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int headerFontSize = 22;
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorLabel = 0x70a7c2ff;

    // -- BACKGROUND --
    auto tex = tm->GetTexture(SpriteFileDialogAutoAttSettings);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // -- BUTTON CLOSE --
    mBtnClose = new ButtonDialogClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    mBtnClose->SetX(buttonX);

    // -- CONTENT --
    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("AUTO_ATT"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // PANEL CONTENT
    const int marginFieldV = 100;

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, 20, graphic::Font::NORMAL);
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText, 18, graphic::Font::NORMAL);

    int x = WidgetsConstants::MarginDialogContentL;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    // ENEMY SELECTION
    sgui::Label * label = new sgui::Label(sm->GetCString("ENEMY_SEL"), fontHeader, this);
    label->SetColor(WidgetsConstants::colorDialogHeader2);
    label->SetPosition(x, y);

    mComboEnemySel = new GameComboBox(this);
    mComboEnemySel->AddItem(new GameComboBoxItem(sm->GetCString("ATT_DISABLED")));
    mComboEnemySel->AddItem(new GameComboBoxItem(sm->GetCString("ENEMY_CLOSEST")));
    mComboEnemySel->AddItem(new GameComboBoxItem(sm->GetCString("ENEMY_RANDOM")));
    mComboEnemySel->SetActiveItem(mObj->GetAttackStrategy());

    x = w - WidgetsConstants::MarginDialogContentL - mComboEnemySel->GetWidth();
    y += (label->GetHeight() - mComboEnemySel->GetHeight()) / 2;
    mComboEnemySel->SetPosition(x, y);

    mComboEnemySel->SetOnActiveChanged([this](int ind)
    {
        const auto as = static_cast<AttackStrategy>(ind);

        mObj->SetAttackStrategy(as);

        const bool enabled = (as != ATTSTR_NO_ATTACK);
        mComboAttackMode->SetEnabled(enabled);
        mSliderEnergy->SetEnabled(enabled);
    });

    // ATTACK MODE
    x = WidgetsConstants::MarginDialogContentL;
    y = label->GetY() + label->GetHeight() + marginFieldV;

    label = new sgui::Label(sm->GetCString("ATT_MODE"), fontHeader, this);
    label->SetColor(WidgetsConstants::colorDialogHeader2);
    label->SetPosition(x, y);

    mComboAttackMode = new GameComboBox(this);
    mComboAttackMode->AddItem(new GameComboBoxItem(sm->GetCString("QUICK_SHOT")));
    mComboAttackMode->AddItem(new GameComboBoxItem(sm->GetCString("AIMED_SHOT")));
    mComboAttackMode->AddItem(new GameComboBoxItem(sm->GetCString("BURST_SHOT")));
    mComboAttackMode->SetActiveItem(mObj->GetWeapon()->GetAttackMode());

    x = w - WidgetsConstants::MarginDialogContentL - mComboAttackMode->GetWidth();
    y += (label->GetHeight() - mComboAttackMode->GetHeight()) / 2;
    mComboAttackMode->SetPosition(x, y);

    mComboAttackMode->SetOnActiveChanged([this](int ind)
    {
        mObj->SetAttackMode(static_cast<AttackMode>(ind));
    });

    // ENERGY TO SAVE
    const int energy = mObj->GetEnergyToSave();

    x = WidgetsConstants::MarginDialogContentL;
    y = label->GetY() + label->GetHeight() + marginFieldV;

    label = new sgui::Label(sm->GetCString("ENERGY_2_SAVE"), fontHeader, this);
    label->SetColor(WidgetsConstants::colorDialogHeader2);
    label->SetPosition(x, y);

    const int minEne = 0;
    const int maxEne = mObj->GetMaxEnergy();
    mSliderEnergy = new StandardGameSliderH(this);

    mSliderEnergy->SetMinMax(minEne, maxEne);
    mSliderEnergy->SetValue(energy);

    auto labelVal = new sgui::Label(std::to_string(maxEne).c_str(), fontHeader, this);
    labelVal->SetColor(WidgetsConstants::colorDialogText);

    x = w - WidgetsConstants::MarginDialogContentL - labelVal->GetWidth();
    y += (label->GetHeight() - labelVal->GetHeight()) / 2;
    labelVal->SetPosition(x, y);

    labelVal->SetText(std::to_string(energy).c_str());

    const int marginSliderH = 20;
    x -= marginSliderH + mSliderEnergy->GetWidth();
    y += (labelVal->GetHeight() - mSliderEnergy->GetHeight()) / 2;
    mSliderEnergy->SetPosition(x, y);

    mSliderEnergy->AddOnValueChanged([this, labelVal](int val)
    {
        mObj->SetEnergyToSave(val);

        labelVal->SetText(std::to_string(val).c_str());
    });
}

void DialogAutoAttackSettings::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogAutoAttackSettings::HandlePositionChanged()
{
    SetPositions();
}

void DialogAutoAttackSettings::SetPositions()
{
    const int y = GetScreenY();
    int x = GetScreenX();

    // BACKGROUND
    mBg->SetPosition(x, y);
}

} // namespace game
