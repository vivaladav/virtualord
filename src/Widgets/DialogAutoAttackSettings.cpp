#include "Widgets/DialogAutoAttackSettings.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/ResearchCenter.h"
#include "Widgets/ButtonDialogClose.h"
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

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // -- BUTTON CLOSE --
    mBtnClose = new ButtonDialogClose(this);

    const int buttonX = GetWidth() - mBtnClose->GetWidth();
    mBtnClose->SetX(buttonX);

    // -- CONTENT --
    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, 20, graphic::Font::NORMAL);
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText, 18, graphic::Font::NORMAL);

    int valRes = 0;

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("AUTO_ATT"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    int x = WidgetsConstants::MarginDialogContentL;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;
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
