#include "Screens/ScreenTest.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameTestData.h"
#include "States/StatesIds.h"
#include "Widgets/ButtonMainMenu.h"
#include "Widgets/ButtonUnitsSelector.h"
#include "Widgets/Test/TestComboBox.h"
#include "Widgets/Test/TestProgressBar.h"
#include "Widgets/Test/TestSliderH.h"

#include <sgl/core/Timer.h>
#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/core/event/MouseButtonEvent.h>
#include <sgl/graphic/Cursor.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/Filesystem.h>

#include <chrono>
#include <iostream>

namespace game
{

const float TIME_PB = 4.f;

ScreenTest::ScreenTest(Game * game)
    : Screen(game)
{
    using namespace sgl::graphic;

    game->SetClearColor(0x22, 0x22, 0x22, 0xFF);

    // -- Image testing --
    auto tm = TextureManager::Instance();
    Texture * tex = nullptr;
    Image * img = nullptr;

    tex = tm->GetTexture("test/obj_null.png");
    img = new Image(tex);
    img->SetPosition(64, 32);
    mRenderables.emplace_back(img);

    img = new Image(tex);
    img->SetPosition(128, 64);
    img->SetAlpha(128);
    mRenderables.emplace_back(img);

    img = new Image(tex);
    img->SetPosition(250, 32);
    img->ScaleH(2.f);
    img->SetRotation(45.f);
    img->SetColor(0x3366F0FF);
    mRenderables.emplace_back(img);

    img = new Image(tex);
    img->SetPosition(32, 64);
    img->SetWidth(64);
    img->SetHeight(32);
    mRenderables.emplace_back(img);

    // -- Text testing --
    FontManager * fm = FontManager::Instance();

    const int TXT_X0 = 20;
    Font * font = fm->GetFont("Lato-Regular.ttf", 24, Font::NORMAL);
    Text * txt = nullptr;

    txt = new Text("Text 1", font);
    txt->SetPosition(TXT_X0, 250);
    mRenderables.emplace_back(txt);

    font = fm->GetFont("Lato-Regular.ttf", 24, Font::BOLD | Font::ITALIC);
    txt = new Text("Text 2 (bold & italic)", font);
    txt->SetPosition(TXT_X0, 300);
    mRenderables.emplace_back(txt);

    font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);
    txt = new Text("Text 3 (bold - A=128)", font);
    txt->SetPosition(TXT_X0, 350);
    txt->SetAlpha(128);
    mRenderables.emplace_back(txt);

    font = fm->GetFont("Lato-Regular.ttf", 24, Font::NORMAL);
    txt = new Text("Text 4 (50, 150, 250, 255)", font);
    txt->SetPosition(TXT_X0, 400);
    txt->SetColor(50, 150, 250, 255);
    mRenderables.emplace_back(txt);

    txt = new Text("Text 5 (50, 150, 250, 128)", font);
    txt->SetPosition(TXT_X0, 450);
    txt->SetColor(50, 150, 250, 128);
    mRenderables.emplace_back(txt);

    // -- cursor --
    tex = tm->GetSprite(SpriteFileTestUI, IND_TC_CURSOR1);
    mCursor = new Cursor(tex, 2, 0);

    sgl::sgui::Stage::Instance()->SetCursor(mCursor);

    // -- TESTS --
    TestSprite();

    TestRotation();

    TestSGui();

    TestTimer();

    TestAudio();

    TestUtilities();
}

ScreenTest::~ScreenTest()
{
    sgl::sgui::Stage::Instance()->ClearCursor();
    delete mCursor;

    delete mTimer1;
    delete mTimer2;

    for(sgl::graphic::TexturedRenderable * r : mRenderables)
        delete r;

    sgl::sgui::Stage::Instance()->ClearWidgets();

    GetGame()->SetCurrentCursor(CURSOR_DEFAULT);
}

void ScreenTest::OnKeyUp(sgl::core::KeyboardEvent & event)
{
    const int key = event.GetKey();

    // ESC -> go back to main menu
    if(key == sgl::core::KeyboardEvent::KEY_ESCAPE)
        GetGame()->RequestNextActiveState(StateId::MAIN_MENU);
    if(key == sgl::core::KeyboardEvent::KEY_C)
    {
        if(event.IsModShiftDown())
            sgl::sgui::Stage::Instance()->ShowCursor();
        else if(event.IsModCtrlDown())
            sgl::sgui::Stage::Instance()->HideCursor();
    }
}

void ScreenTest::Update(float delta)
{
    if(mTimerPb < TIME_PB)
    {
        mTimerPb += delta;

        const float perc = mTimerPb * 100.f / TIME_PB;

        mPb0->SetValuePerc(perc);
        mPb1->SetValuePerc(perc);
        mPb2->SetValuePerc(perc);
    }
}

void ScreenTest::Render()
{
    for(sgl::graphic::TexturedRenderable * r : mRenderables)
        r->Render();
}

void ScreenTest::OnMouseButtonUp(sgl::core::MouseButtonEvent & event)
{
    std::cout << "Mouse clicked - button: " << event.GetButton()
              << " - pos: " << event.GetX() << " , " << event.GetY() << std::endl;
}

void ScreenTest::TestSGui()
{
    using namespace sgl::graphic;
    using namespace sgl::sgui;

    const int Y0 = 20;
    const int marginV = 50;

    Widget * container = new Widget;
    container->SetPosition(400, Y0);

    FontManager * fm = FontManager::Instance();

    // -- PUSHBUTTON --
    Font * font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);
    Label * label = new Label("PUSH BUTTON", font, container);

    ButtonMainMenu * button = new ButtonMainMenu("BUTTON 1", container);
    button->SetY(label->GetHeight() + 20);

    font = fm->GetFont("Lato-Regular.ttf", 24, Font::NORMAL);
    label = new Label(font, container);
    label->SetY(button->GetY() + button->GetHeight() + 10);

    button->AddOnClickFunction([label]
    {
        label->SetText("button 1 clicked");
        std::cout << "button 1 clicked" << std::endl;

        sgl::media::AudioManager::Instance()->GetPlayer()->PlaySound("test/test.ogg");
    });

    auto labelTT = new Label("ButtonMainMenu", font);
    labelTT->SetColor(0xEEEEEEFF);
    button->SetTooltip(labelTT);

    button = new ButtonMainMenu("BUTTON 2", container);
    button->SetY(label->GetY() + marginV);

    button->AddOnClickFunction([label]
    {
        label->SetText("button 2 clicked");
        std::cout << "button 2 clicked" << std::endl;
    });

    int buttonY = button->GetY() + button->GetHeight() + marginV;

    button = new ButtonMainMenu("BUTTON 3", container);
    button->SetEnabled(false);
    button->SetY(buttonY);

    buttonY += button->GetHeight() + marginV;

    button = new ButtonMainMenu("SELECTED", container);
    button->SetCheckable(true);
    button->SetY(buttonY);

    buttonY += button->GetHeight() + marginV;

    // -- TEXT AREA --
    font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);
    label = new Label("TEXT AREA", font, container);
    label->SetY(buttonY);

    buttonY += label->GetHeight() * 1.5f;

    button = new ButtonMainMenu("CHANGE TEXT", container);
    button->SetY(label->GetY() + marginV);

    buttonY += button->GetHeight() * 1.5f;

    auto tm = TextureManager::Instance();
    auto borders = new sgl::sgui::Image(tm->GetTexture("test/text_area.png"), container);
    borders->SetPosition(0, buttonY);

    const int taW = 400;
    const int taH = 300;
    const char * text = "This is a pretty long text.(NN)\n\nOver multiple lines, well, at least 4 and I bet that "
                        "this line will wrap at some point before the newline.(N)\nHere it is the end!";
    font = fm->GetFont("Lato-Regular.ttf", 22, Font::NORMAL);
    auto ta = new TextArea(taW, taH, text, font, false, container);
    ta->SetY(buttonY);
    ta->SetColor(0x99FFAAFF);

    button->AddOnClickFunction([ta]
    {
        ta->SetText("Text changed after pushing a button. All good so far. Also NO newline in this case!");
    });

    ButtonsGroup * bgAlignH = new ButtonsGroup(ButtonsGroup::HORIZONTAL, container);
    bgAlignH->AddButton(new ButtonUnitsSelector("L"));
    bgAlignH->AddButton(new ButtonUnitsSelector("C"));
    bgAlignH->AddButton(new ButtonUnitsSelector("R"));
    bgAlignH->SetButtonChecked(0, true);
    bgAlignH->SetPosition(ta->GetX(), ta->GetY() + ta->GetHeight());

    bgAlignH->SetFunctionOnToggle([ta](unsigned int ind, bool checked)
    {
       if(checked)
           ta->setTextAlignmentHorizontal(static_cast<TextArea::Alignment>(ind));
    });

    ButtonsGroup * bgAlignV = new ButtonsGroup(ButtonsGroup::HORIZONTAL, container);
    bgAlignV->AddButton(new ButtonUnitsSelector("T"));
    bgAlignV->AddButton(new ButtonUnitsSelector("C"));
    bgAlignV->AddButton(new ButtonUnitsSelector("B"));
    bgAlignV->SetButtonChecked(0, true);
    bgAlignV->SetPosition(ta->GetX() + ta->GetWidth() - bgAlignV->GetWidth(),
                          ta->GetY() + ta->GetHeight());

    bgAlignV->SetFunctionOnToggle([ta](unsigned int ind, bool checked)
    {
       if(checked)
           ta->setTextAlignmentVertical(static_cast<TextArea::Alignment>(ind + TextArea::ALIGN_V_TOP));
    });

    // ===== COL 3 =====
    // -- BUTTONS GROUP --
    const int col3X = 500;

    font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);

    label = new Label("BUTTONS GROUP", font, container);
    label->SetX(col3X);

    ButtonsGroup * bg = new ButtonsGroup(ButtonsGroup::HORIZONTAL, container);
    bg->SetPosition(label->GetX(), label->GetHeight() + 50);

    bg->AddButton(new ButtonUnitsSelector("1"));
    bg->AddButton(new ButtonUnitsSelector("2"));
    bg->AddButton(new ButtonUnitsSelector("3"));
    bg->AddButton(new ButtonUnitsSelector("4"));

    // set initial checked
    bg->SetButtonChecked(0, true);

    // test set checked on disabled
    bg->SetButtonEnabled(3, false);
    bg->SetButtonChecked(3, true);

    label = new Label("1", font, container);
    label->SetPosition(bg->GetX() + bg->GetWidth() * 1.1f,
                       bg->GetY() + (bg->GetHeight() - label->GetHeight()) * 0.5f);

    for(unsigned int n = 0; n < bg->GetNumButtons(); ++n)
    {
        auto btn = static_cast<ButtonUnitsSelector *>(bg->GetButton(n));
        btn->AddOnToggleFunction([label, n](bool checked)
        {
            if(checked)
                label->SetText(std::to_string(n + 1).c_str());
        });
    }

    // -- PROGRESS BAR --
    const float minProgress = 0.f;
    const float maxProgress = 100.f;

    font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);

    label = new Label("PROGRESS BAR", font, container);
    label->SetPosition(col3X, 160);

    const int pbX = label->GetX();
    const int pbY = label->GetY() + label->GetHeight() + 50;
    const int marginbBars = 25;

    mPb0 = new TestProgressBar(FACTION_1, minProgress, maxProgress, container);
    mPb0->SetPosition(pbX, pbY);
    mPb0->SetValue(0);
    mPb0->AddFunctionOnCompleted([]{ std::cout << "TEST PROGRESS 0 100%" << std::endl; });

    mPb1 = new TestProgressBar(FACTION_2, minProgress, maxProgress, container);
    mPb1->SetPosition(pbX, pbY + mPb0->GetHeight() + marginbBars);
    mPb1->SetValue(0);
    mPb1->AddFunctionOnCompleted([]{ std::cout << "TEST PROGRESS 1 100%" << std::endl; });

    mPb2 = new TestProgressBar(FACTION_3, minProgress, maxProgress, container);
    mPb2->SetPosition(pbX, mPb1->GetY() + mPb1->GetHeight() + marginbBars);
    mPb2->SetValue(0);
    mPb2->AddFunctionOnCompleted([]{ std::cout << "TEST PROGRESS 2 100%" << std::endl; });

    button = new ButtonMainMenu("RESET", container);
    button->SetPosition(mPb0->GetX() + mPb0->GetWidth() + marginbBars * 2, mPb0->GetY());
    button->AddOnClickFunction([this]
    {
        mTimerPb = 0;
        mPb0->Reset();
        mPb1->Reset();
        mPb2->Reset();
    });

    // -- SET ALPHA --
    int wX = 900;
    int wY = button->GetY() + button->GetHeight() + 70;

    auto container2 = new Widget;
    container2->SetPosition(wX, wY);

    label = new Label("SET ALPHA", font, container2);
    wY = label->GetHeight() * 2;

    button = new ButtonMainMenu("ALPHA 50%", container2);
    button->SetY(wY);
    button->AddOnClickFunction([container2]{ container2->SetAlpha(128); });

    button = new ButtonMainMenu("ALPHA 100%", container2);
    wY += button->GetHeight() * 1.2f;
    button->SetY(wY);
    button->AddOnClickFunction([container2]{ container2->SetAlpha(255); });

    // -- COMBO BOX --
    wY = 640;

    auto container3 = new Widget;
    container3->SetPosition(wX, wY);

    label = new Label("COMBO BOX", font, container3);
    wY = label->GetHeight() * 2;

    auto cb = new TestComboBox(container3);
    cb->AddItem(new TestComboBoxItem("OPTION 1"));
    cb->AddItem(new TestComboBoxItem("OPTION 2"));
    cb->AddItem(new TestComboBoxItem("OPTION 3"));

    cb->SetY(wY);

    font = fm->GetFont("Lato-Regular.ttf", 22, Font::NORMAL);
    label = new Label(cb->GetActiveItem()->GetText().c_str(), font, container3);
    label->SetPosition(cb->GetX() + cb->GetWidth() + 50, cb->GetY());

    cb->SetOnActiveChanged([label, cb](int index)
    {
        label->SetText(cb->GetActiveItem()->GetText().c_str());
    });

    // SLIDER
    wY = 800;
    auto container4 = new Widget;
    container4->SetPosition(wX, wY);

    font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);
    label = new Label("SLIDER", font, container4);
    wY = label->GetHeight() * 2;

    // SLIDER DEFAULT
    auto slider = new TestSliderH(container4);
    slider->SetY(wY);

    font = fm->GetFont("Lato-Regular.ttf", 22, Font::NORMAL);
    label = new Label(std::to_string(slider->GetValue()).c_str(), font, container4);
    label->SetPosition(slider->GetX() + slider->GetWidth() + 50, slider->GetY());

    slider->SetOnValueChanged([label](int val)
    {
        label->SetText(std::to_string(val).c_str());
    });

    wY += slider->GetHeight() * 2;

    // SLIDER CUSTOM RANGE
    slider = new TestSliderH(container4);
    slider->SetMinMax(50, 500);
    slider->SetStep(50);
    slider->SetY(wY);

    font = fm->GetFont("Lato-Regular.ttf", 22, Font::NORMAL);
    label = new Label(std::to_string(slider->GetValue()).c_str(), font, container4);
    label->SetPosition(slider->GetX() + slider->GetWidth() + 50, slider->GetY());

    slider->SetOnValueChanged([label](int val)
    {
        label->SetText(std::to_string(val).c_str());
    });

    wY += slider->GetHeight() * 2;

    // SLIDER TEST STEP
    slider = new TestSliderH(container4);
    slider->SetMinMax(0, 25);
    slider->SetStep(10);
    slider->SetY(wY);

    label = new Label(std::to_string(slider->GetValue()).c_str(), font, container4);
    label->SetPosition(slider->GetX() + slider->GetWidth() + 50, slider->GetY());

    slider->SetOnValueChanged([label](int val)
                              {
                                  label->SetText(std::to_string(val).c_str());
                              });

    // ===== COL 4 =====
    // SCROLL BAR
    wX = 1450;
    wY = Y0;
    auto container5 = new Widget;
    container5->SetPosition(wX, wY);

    font = fm->GetFont("Lato-Bold.ttf", 24, Font::NORMAL);
    label = new Label("SCROLLBAR", font, container5);
    wY = label->GetHeight() * 2;
}

void ScreenTest::TestSprite()
{
    using namespace sgl::graphic;

    auto tm = TextureManager::Instance();

    const int x0 = 20;
    const int y0 = 550;
    const int marginH = 20;
    const int marginV = 20;

    Image * img = nullptr;

    img = new Image(tm->GetTexture(SpriteFileTestSprite));
    mRenderables.emplace_back(img);

    img->SetPosition(x0, y0);

    const int x1 = x0 + img->GetWidth() + marginH;

    // row 1
    int x = x1;

    for(unsigned int i = 0; i < NUM_TEST_SPRITE_RECTS; ++i)
    {
        img = new Image(tm->GetSprite(SpriteFileTestSprite, i));
        img->SetPosition(x, y0);
        mRenderables.emplace_back(img);

        x += img->GetWidth() + marginH;
    }

    // row 2
    x = x1;
    const int y1 = y0 + img->GetHeight() + marginV;

    for(unsigned int i = 0; i < NUM_TEST_SPRITE_RECTS; ++i)
    {
        img = new Image(tm->GetSprite(SpriteFileTestSprite, NUM_TEST_SPRITE_RECTS - i - 1));
        img->SetPosition(x, y1);
        mRenderables.emplace_back(img);

        x += img->GetWidth() + marginH;
    }
}

void ScreenTest::TestRotation()
{
    using namespace sgl::graphic;

    auto tm = TextureManager::Instance();
    Texture * tex = nullptr;
    Image * img = nullptr;



    // no bg bar
    img = new Image(tm->GetTexture("test/square100.png"));
    img->SetPosition(50, 800);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/test-bar-nobg.png"));
    img->SetPosition(50, 800);
    img->SetRotation(45.f);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/red_dot4.png"));
    img->SetPosition(50, 800);
    mRenderables.emplace_back(img);

    // bar in square
    img = new Image(tm->GetTexture("test/square100.png"));
    img->SetPosition(200, 800);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/test-bar-bg.png"));
    img->SetPosition(200, 800);
    img->SetRotation(45.f);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/red_dot4.png"));
    img->SetPosition(200, 800);
    mRenderables.emplace_back(img);

    // no bg bar - centered in origin
    img = new Image(tm->GetTexture("test/square100.png"));
    img->SetPosition(50, 950);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/test-bar-nobg.png"));
    img->SetPosition(50 - img->GetWidth() * 0.5f, 950- img->GetHeight() * 0.5f);
    img->SetRotation(45.f);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/red_dot4.png"));
    img->SetPosition(50, 950);
    mRenderables.emplace_back(img);

    // bar in square - centered in origin
    img = new Image(tm->GetTexture("test/square100.png"));
    img->SetPosition(200, 950);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/test-bar-bg.png"));
    img->SetPosition(200 - img->GetWidth() * 0.5f, 950- img->GetHeight() * 0.5f);
    img->SetRotation(45.f);
    mRenderables.emplace_back(img);

    img = new Image(tm->GetTexture("test/red_dot4.png"));
    img->SetPosition(200, 950);
    mRenderables.emplace_back(img);
}

void ScreenTest::TestTimer()
{
    using namespace sgl;

    // SINGLE SHOT TIMER
    mTimer1 = new core::Timer(2.f);
    mTimer1->SetSingleShot(true);

    auto t0 = std::chrono::high_resolution_clock::now();

    mTimer1->AddTimeoutFunction([t0]
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> diff = t1 - t0;
        std::cout << "mTimer1(single shot) - duration: " << diff.count() << std::endl;
    });

    mTimer1->Start();

    // TIMER
    mTimer2 = new core::Timer(5.f);

    t0 = std::chrono::high_resolution_clock::now();

    mTimer2->AddTimeoutFunction([t0]
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float> diff = t1 - t0;
        std::cout << "mTimer2 - duration: " << diff.count() << std::endl;
    });

    mTimer2->Start();
}

void ScreenTest::TestAudio()
{
    auto am = sgl::media::AudioManager::Instance();
    auto ap = am->GetPlayer();

    ap->StopMusic();
    ap->AddMusicToQueue("test/test.ogg");
    ap->AddMusicToQueue("test/menu_01.ogg");
    ap->PlayMusicQueue();
}

void ScreenTest::TestUtilities()
{
    sgl::utilities::Filesystem fs;
    std::cout << "HOME DIR: " << fs.GetUserHomeDirectory() << std::endl;
}

} // namespace game
