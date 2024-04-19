#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>

namespace game
{

const int marginSide = 25;

const float timeContinue = 1.5f;

PanelInfoTutorial::PanelInfoTutorial(int w, int h)
{
    using namespace sgl;

    SetSize(w, h);

    // CREATE BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = nullptr;

    const unsigned int texIds[NUM_BGPARTS] =
    {
        IND_TUT_PANEL_INFO_CORNER_TL,
        IND_TUT_PANEL_INFO_CORNER_TR,
        IND_TUT_PANEL_INFO_CORNER_BL,
        IND_TUT_PANEL_INFO_CORNER_BR,
        IND_TUT_PANEL_INFO_BG,
        IND_TUT_PANEL_INFO_L,
        IND_TUT_PANEL_INFO_R,
        IND_TUT_PANEL_INFO_T,
        IND_TUT_PANEL_INFO_B
    };

    for(unsigned int ind = BGPART_TL; ind < NUM_BGPARTS; ++ind)
    {
        tex = tm->GetSprite(SpriteFileTutorialExp, texIds[ind]);
        tex->SetScaleMode(0);

        mBgParts[ind] = new graphic::Image(tex);
        RegisterRenderable(mBgParts[ind]);
    }

    // LABEL CONTINUE
    const int marginBottom = 20;

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont("Lato-Regular.ttf", 18, graphic::Font::NORMAL);

    mLabelContinue = new sgui::Label("press SPACE to continue", font, this);
    mLabelContinue->SetColor(0x6c8093ff);

    const int labelX = (w - mLabelContinue->GetWidth()) / 2;
    const int labelY = h - marginBottom - mLabelContinue->GetHeight();
    mLabelContinue->SetPosition(labelX, labelY);

    // POSITION ELEMENTS
    PositionElements();
}

void PanelInfoTutorial::AddInfoEntry(const char * text, unsigned int color, float timeNext,
                                     bool showContinue, bool hideAfter)
{
    using namespace sgl;

    // NEW ENTRY
    auto entry = new InfoEntry;

    // text area
    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont("Lato-Regular.ttf", 20, graphic::Font::NORMAL);

    const int areaW = GetWidth() - (2 * marginSide);
    const int areaH = 0;

    entry->mTxtArea = new sgui::TextArea(areaW, areaH, text, font, true, this);
    entry->mTxtArea->SetColor(color);
    entry->mTxtArea->SetVisible(false);

    // auto time
    entry->mTimeNext = timeNext;

    // options
    entry->mAutoContinue = timeNext > 0.f;
    entry->mShowContinue = showContinue;
    entry->mHideAfter = hideAfter;

    // store
    mInfoEntries.push_back(entry);
}

void PanelInfoTutorial::StartInfo()
{
    if(mInfoEntries.empty())
        return ;

    const int marginTop = 25;
    mCurrEntryY = marginTop;

    mCurrEntry = 0;
    ShowCurrentInfo();
}

void PanelInfoTutorial::ShowNextInfo()
{
    if(mInfoEntries.empty())
        return ;

    auto oldEntry = mInfoEntries[mCurrEntry];

    // hide current entry
    if(oldEntry->mHideAfter)
        oldEntry->mTxtArea->SetVisible(false);
    // keep current entry
    else
    {
        oldEntry->mTxtArea->SetColor(colorTutorialTextOld);

        const int marginTextV = 25;
        mCurrEntryY += oldEntry->mTxtArea->GetTextHeight() + marginTextV;
    }

    // next
    ++mCurrEntry;
    ShowCurrentInfo();
}

void PanelInfoTutorial::ShowCurrentInfo()
{
    // show text
    auto entry = mInfoEntries[mCurrEntry];

    const int entryX = marginSide;

    entry->mTxtArea->SetVisible(true);
    entry->mTxtArea->SetPosition(entryX, mCurrEntryY);

    // start auto continue
    if(entry->mAutoContinue)
        mTimerNextEntry = entry->mTimeNext;

    // do not show continue immediately
    mLabelContinue->SetVisible(false);
    mTimerContinue = timeContinue;

    // check for finished
    if(!entry->mShowContinue && mCurrEntry == (mInfoEntries.size() - 1))
        mOnFinished();
}

void PanelInfoTutorial::HandleKeyUp(sgl::core::KeyboardEvent & event)
{
    if(event.GetKey() == sgl::core::KeyboardEvent::KEY_SPACE)
    {
        if(mCurrEntry < (mInfoEntries.size() - 1))
        {
            ShowNextInfo();

            event.SetConsumed();
        }
        else if(mCurrEntry < mInfoEntries.size())
        {
            ++mCurrEntry;
            mOnFinished();

            event.SetConsumed();
        }
    }
}

void PanelInfoTutorial::HandlePositionChanged()
{
    PositionElements();
}

void PanelInfoTutorial::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = GetWidth();
    const int h = GetHeight();
    const int cornerW = mBgParts[BGPART_TL]->GetWidth();
    const int cornerH = mBgParts[BGPART_TL]->GetHeight();

    // -- BACKGROUND --
    int x = x0;
    int y = y0;
    int pW, pH;

    // TL
    mBgParts[BGPART_TL]->SetPosition(x, y);

    // T
    x += cornerW;
    pW = w - (2 * cornerW);

    mBgParts[BGPART_T]->SetPosition(x, y);
    mBgParts[BGPART_T]->SetWidth(pW);

    // TR
    x += pW;

    mBgParts[BGPART_TR]->SetPosition(x, y);

    // L
    x = x0;
    y += cornerH;
    pH = h - (2 * cornerH);

    mBgParts[BGPART_L]->SetPosition(x, y);
    mBgParts[BGPART_L]->SetHeight(pH);

    // CENTER
    x += cornerW;
    pW = w - (2 * cornerW);
    pH = h - (2 * cornerH);

    mBgParts[BGPART_CENTER]->SetPosition(x, y);
    mBgParts[BGPART_CENTER]->SetWidth(pW);
    mBgParts[BGPART_CENTER]->SetHeight(pH);

    // R
    x += pW;
    pH = h - (2 * cornerH);

    mBgParts[BGPART_R]->SetPosition(x, y);
    mBgParts[BGPART_R]->SetHeight(pH);

    // BL
    x = x0;
    y += pH;

    mBgParts[BGPART_BL]->SetPosition(x, y);

    // B
    x += cornerW;
    pW = w - (2 * cornerW);

    mBgParts[BGPART_B]->SetPosition(x, y);
    mBgParts[BGPART_B]->SetWidth(pW);

    // BR
    x += pW;

    mBgParts[BGPART_BR]->SetPosition(x, y);
}

void PanelInfoTutorial::OnUpdate(float delta)
{
    if(mInfoEntries.empty() || mCurrEntry == mInfoEntries.size())
        return ;

    auto entry = mInfoEntries[mCurrEntry];

    // need to show continue
    if(entry->mShowContinue && !mLabelContinue->IsVisible())
    {
        mTimerContinue -= delta;

        if(mTimerContinue <= 0.f)
            mLabelContinue->SetVisible(true);
    }

    // auto continue
    if(entry->mAutoContinue)
    {
        mTimerNextEntry -= delta;

        if(mTimerNextEntry <= 0.f)
        {
            if(mCurrEntry < (mInfoEntries.size() - 1))
                ShowNextInfo();
            else
            {
                ++mCurrEntry;
                mOnFinished();
            }
        }
    }
}

} // namespace game
