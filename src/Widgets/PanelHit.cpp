#include "PanelHit.h"

#include "GameObjects/GameObject.h"
#include "GameObjectTools/Weapon.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <iomanip>
#include <sstream>
#include <string>

namespace game
{

PanelHit::PanelHit()
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    sm->AddListener(this);

    const int sizeHeader = 15;
    auto fontHeader = fm->GetFont(WidgetsConstants::FontFilePanelHeader,
                                  sizeHeader, graphic::Font::NORMAL);

    const int sizeText = 16;
    auto fontText = fm->GetFont(WidgetsConstants::FontFilePanelText,
                                sizeText, graphic::Font::NORMAL);

    const int marginT = 5;
    const int marginBlockB = 35;
    int y = marginT;

    // BACKGROUND
    graphic::Texture * tex = tm->GetTexture(SpriteFilePanelHit);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // HIT
    mHeaderHit = new sgui::Label(sm->GetCString("PH_HIT"), fontHeader, this);
    mHeaderHit->SetColor(WidgetsConstants::colorPanelHeader);
    mHeaderHit->SetPosition((w - mHeaderHit->GetWidth()) / 2, y);

    y += mHeaderHit->GetHeight();

    mLabelHit = new sgui::Label(fontText, this);
    mLabelHit->SetY(y);

    y += marginBlockB;

    // FATAL
    mHeaderFatal = new sgui::Label(sm->GetCString("PH_FATAL"), fontHeader, this);
    mHeaderFatal->SetColor(WidgetsConstants::colorPanelHeader);
    mHeaderFatal->SetPosition((w - mHeaderFatal->GetWidth()) / 2, y);

    y += mHeaderFatal->GetHeight();

    mLabelFatal = new sgui::Label(fontText, this);
    mLabelFatal->SetY(y);

    y += marginBlockB;

    // COST
    mHeaderCost = new sgui::Label(sm->GetCString("PH_COST"), fontHeader, this);
    mHeaderCost->SetColor(WidgetsConstants::colorPanelHeader);
    mHeaderCost->SetPosition((w - mHeaderCost->GetWidth()) / 2, y);

    y += mHeaderCost->GetHeight();

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_W_RES_ENERGY_16);
    mIconCost = new sgui::Image(tex, this);
    mIconCost->SetColor(WidgetsConstants::colorPanelHeader);
    mIconCost->SetY(y);

    mLabelCost = new sgui::Label(fontText, this);
}

void PanelHit::ShowAttackerData(const GameObject * attacker, const GameObject * target)
{
    auto weapon = attacker->GetWeapon();

    if(weapon == nullptr)
        return ;

    const float probBad = 40.f;
    const float probWarning = 60.f;

    const std::string strEmpty;
    const std::string strPerc("%");
    const int prec = 2;
    std::ostringstream oss;

    const int w = GetWidth();

    // HIT
    const float probHit = weapon->GetProbabilityHit(target);

    oss << std::fixed << std::setprecision(prec) << probHit  << strPerc;
    mLabelHit->SetText(oss.str().c_str());
    mLabelHit->SetX((w - mLabelHit->GetWidth()) / 2);

    if(probHit < probBad)
        mLabelHit->SetColor(WidgetsConstants::colorPanelBad);
    else if(probHit < probWarning)
        mLabelHit->SetColor(WidgetsConstants::colorPanelWarning);
    else
        mLabelHit->SetColor(WidgetsConstants::colorPanelGood);

    oss.clear();
    oss.str(strEmpty);

    // FATAL
    const float probFatal = weapon->GetProbabilityFatalHit(target);

    oss << std::fixed << std::setprecision(prec) << probFatal << strPerc;
    mLabelFatal->SetText(oss.str().c_str());
    mLabelFatal->SetX((w - mLabelFatal->GetWidth()) / 2);

    if(probFatal < probBad)
        mLabelFatal->SetColor(WidgetsConstants::colorPanelBad);
    else if(probFatal < probWarning)
        mLabelFatal->SetColor(WidgetsConstants::colorPanelWarning);
    else
        mLabelFatal->SetColor(WidgetsConstants::colorPanelGood);

    oss.clear();
    oss.str(strEmpty);

    // COST
    const int marginR = 5;

    const int cost = weapon->GetCostEnergy();
    const int attackerEnergy = attacker->GetEnergy();
    const unsigned int colorCost = cost > attackerEnergy ? WidgetsConstants::colorPanelBad :
                                   WidgetsConstants::colorPanelGood;

    oss << std::fixed << std::setprecision(prec) << cost;
    mLabelCost->SetText(oss.str().c_str());
    mLabelCost->SetColor(colorCost);

    const int rowW = mLabelCost->GetWidth() + marginR + mIconCost->GetWidth();
    int x = (w - rowW) / 2;
    int y = mIconCost->GetY() + (mIconCost->GetHeight() - mLabelCost->GetHeight()) / 2;
    mLabelCost->SetPosition(x, y);

    x += mLabelCost->GetWidth() + marginR;

    mIconCost->SetX(x);
}

void PanelHit::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelHit::UpdatePositions()
{
    mBg->SetPosition(GetScreenX(), GetScreenY());
}

void PanelHit::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    const int w = GetWidth();

    mHeaderHit->SetText(sm->GetCString("PH_HIT"));
    mHeaderHit->SetX((w - mHeaderHit->GetWidth()) / 2);

    mHeaderFatal->SetText(sm->GetCString("PH_FATAL"));
    mHeaderFatal->SetX((w - mHeaderFatal->GetWidth()) / 2);

    mHeaderCost->SetText(sm->GetCString("PH_COST"));
    mHeaderCost->SetX((w - mHeaderCost->GetWidth()) / 2);
}

} // namespace sgl
