#include "ObjectVisualAttribute.h"

#include "Widgets/GameUIData.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>

namespace game
{

ObjectVisualAttribute::ObjectVisualAttribute(sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();

    // BACKGROUND
    auto tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ATT_OFF);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // LABEL
    auto font = fm->GetFont("Lato-Regular.ttf", 18, graphic::Font::NORMAL);
    mLabel = new sgui::Label(font, this);
    mLabel->SetColor(0xf1f2f4ff);
    mLabel->SetVisible(false);

    // VALUE BAR
    mValueBar = new sgui::Image(this);
    mValueBar->SetVisible(false);
}

void ObjectVisualAttribute::ClearData()
{
    // check if already cleared
    if(!mLabel->IsVisible())
        return ;

    // BACKGROUND
    auto tm = sgl::graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ATT_OFF);
    mBg->SetTexture(tex);

    // CONTENT
    mLabel->SetVisible(false);
    mValueBar->SetVisible(false);
}

void ObjectVisualAttribute::SetData(const char * txt, unsigned int val)
{
    using namespace sgl::graphic;

    // BACKGROUND
    auto tm = TextureManager::Instance();
    Texture * tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ATT_ON);
    mBg->SetTexture(tex);

    // LABEL
    mLabel->SetText(txt);
    mLabel->SetVisible(true);

    // VALUE BAR
    const unsigned int maxVal = 10;

    if(val > maxVal)
        val = maxVal;

    const unsigned int texId = IND_DLG_NEWE_BAR0 + val;
    tex = tm->GetSprite(SpriteFileDialogNewElement, texId);
    mValueBar->SetTexture(tex);
    mValueBar->SetVisible(true);

    // reset positions
    HandlePositionChanged();
}

void ObjectVisualAttribute::HandlePositionChanged()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    const int marginH = 10;

    mBg->SetPosition(x0, y0);

    // LABEL
    const int labelY = (h - mLabel->GetHeight()) / 2;
    mLabel->SetPosition(marginH, labelY);

    // BAR
    const int barX = w - marginH - mValueBar->GetWidth();
    const int barY = (h - mValueBar->GetHeight()) / 2;
    mValueBar->SetPosition(barX, barY);
}

} // namespace game
