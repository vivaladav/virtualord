#include "Widgets/ButtonQuickUnitSelection.h"

#include "Game.h"
#include "GameUIData.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Screens/ScreenGame.h"
#include "Widgets/DigitsDisplay.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

const std::array<unsigned int, sgl::sgui::PushButton::NUM_VISUAL_STATES> TEX_IDS =
{
    SpriteIdUnitQuickSel::IND_UQS_BG_NORMAL,
    SpriteIdUnitQuickSel::IND_UQS_BG_DISABLED,
    SpriteIdUnitQuickSel::IND_UQS_BG_MOUSE_OVER,
    SpriteIdUnitQuickSel::IND_UQS_BG_PUSHED,
    SpriteIdUnitQuickSel::IND_UQS_BG_CHECKED
};

const int MAX_BUTTONS = 10;

const int KEYS[MAX_BUTTONS] =
{
    sgl::core::KeyboardEvent::KEY_1,
    sgl::core::KeyboardEvent::KEY_2,
    sgl::core::KeyboardEvent::KEY_3,
    sgl::core::KeyboardEvent::KEY_4,
    sgl::core::KeyboardEvent::KEY_5,
    sgl::core::KeyboardEvent::KEY_6,
    sgl::core::KeyboardEvent::KEY_7,
    sgl::core::KeyboardEvent::KEY_8,
    sgl::core::KeyboardEvent::KEY_9,
    sgl::core::KeyboardEvent::KEY_0
};

const char * KEYS_STR[MAX_BUTTONS] =
{
    "1", "2", "3", "4", "5",
    "6", "7", "8", "9", "0"
};

ButtonQuickUnitSelection::ButtonQuickUnitSelection(int index, ScreenGame * sg, sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton(TEX_IDS, SpriteFileUnitQuickSel, parent)
    , mScreenGame(sg)
    , mShortcutKey(KEYS[index])
{
    // create shortcut label
    auto fm = sgl::graphic::FontManager::Instance();
    auto fontShortcut = fm->GetFont("Lato-Regular.ttf", 12, sgl::graphic::Font::NORMAL);
    mShortcut = new sgl::graphic::Text(KEYS_STR[index], fontShortcut);
    RegisterRenderable(mShortcut);

    // handle button checked
    AddOnToggleFunction([this](bool checked)
    {
        if(!checked)
            return;

        // nothing else to do if unit already selected
        if(mUnit->IsSelected())
            return;

        Player * p = mScreenGame->GetGame()->GetPlayerByFaction(mUnit->GetFaction());
        mScreenGame->ClearSelection(p);
        mScreenGame->SelectObject(mUnit, p);
    });

    // button disabled by default as it has no unit assigned yet
    SetEnabled(false);
}

void ButtonQuickUnitSelection::ClearUnit()
{
    // already no unit
    if(nullptr == mUnit)
        return ;

    mUnit->RemoveFunctionOnValueChanged(mFunValChangedId);
    mFunValChangedId = 0;

    mUnit = nullptr;

    UnregisterRenderable(mImgUnit);
    delete mImgUnit;
    mImgUnit = nullptr;

    UnregisterRenderable(mIconEnergy);
    delete mIconEnergy;
    mIconEnergy = nullptr;

    delete mDisplayEnergy;
    mDisplayEnergy = nullptr;

    UnregisterRenderable(mIconHealth);
    delete mIconHealth;
    mIconHealth = nullptr;

    delete  mDisplayHealth;
    mDisplayHealth = nullptr;

    // disable button
    SetEnabled(false);
}

void ButtonQuickUnitSelection::SetUnit(Unit * unit)
{
    // already set same unit
    if(unit == mUnit)
        return ;

    auto * tm = sgl::graphic::TextureManager::Instance();

    // set unit
    mUnit = unit;

    // update icon
    if(nullptr == mImgUnit)
    {
        mImgUnit = new sgl::graphic::Image;
        RegisterRenderable(mImgUnit);
    }

    const unsigned int faction = unit->GetFaction();
    const GameObjectTypeId type = unit->GetObjectType();
    const unsigned int ind = Unit::TypeToIndex(type);

    const unsigned int texInd = IND_UQS_UNIT1_F1 + (NUM_UQS_UNIT_SPRITE_PER_FACTION * faction) + ind;
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileUnitQuickSel, texInd);

    mImgUnit->SetTexture(tex);

    // energy
    const int digits = 3;
    const int digitsFontSize = 16;
    const std::string suffix("%");

    if(nullptr == mIconEnergy)
    {
        tex = tm->GetSprite(SpriteFileUnitQuickSel, IND_UQS_ICON_ENERGY);
        mIconEnergy = new sgl::graphic::Image(tex);
        RegisterRenderable(mIconEnergy);

        mDisplayEnergy = new DigitsDisplay(digits, digitsFontSize, suffix, this);
    }

    // health
    if(nullptr == mIconHealth)
    {
        tex = tm->GetSprite(SpriteFileUnitQuickSel, IND_UQS_ICON_HEALTH);
        mIconHealth = new sgl::graphic::Image(tex);
        RegisterRenderable(mIconHealth);

        mDisplayHealth = new DigitsDisplay(digits, digitsFontSize, suffix, this);
        mDisplayHealth->SetSuffix(suffix);
    }

    // set initial values
    UpdateValues();

    // track values changing
    mFunValChangedId = mUnit->AddFunctionOnValueChanged([this] { UpdateValues(); });

    // make sure button is enabled
    SetEnabled(true);

    // position stuff
    HandlePositionChanged();
}

void ButtonQuickUnitSelection::OnStateChanged(VisualState state)
{
    sgl::sgui::ImageButton::OnStateChanged(state);

    const unsigned int colorsShortcut[NUM_VISUAL_STATES] =
    {
        0xc7cdd1ff,
        0x5c6870ff,
        0xe3e6e8ff,
        0xabb4baff,
        0xe5e5b3ff
    };

    mShortcut->SetColor(colorsShortcut[state]);
}

void ButtonQuickUnitSelection::HandlePositionChanged()
{
    sgl::sgui::ImageButton::HandlePositionChanged();

    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // shortcut label
    const int marginShortcutH = 5;
    const int marginShortcutV = 3;
    const int shortcutX = x0 + marginShortcutH;
    const int shortcutY = y0 + marginShortcutV;
    mShortcut->SetPosition(shortcutX, shortcutY);

    // no unit -> no graphics to position
    if(nullptr == mUnit)
        return ;

    // unit image
    const int marginIconL = 20;
    const int imgX = x0 + marginIconL;
    const int imgY = y0 + (GetHeight() - mImgUnit->GetHeight()) * 0.5f;
    mImgUnit->SetPosition(imgX, imgY);

    // icons and displays
    const int iconsCenterY = 22;
    const int displayMarginB = 8;

    // icon energy
    const int iconEnCenterX = 99;
    const int iconEnX = x0 + iconEnCenterX - mIconEnergy->GetWidth() * 0.5f;
    const int iconEnY = y0 + iconsCenterY - mIconEnergy->GetHeight() * 0.5f;
    mIconEnergy->SetPosition(iconEnX, iconEnY);

    // digits energy
    const int digitsEnX = iconEnCenterX - mDisplayEnergy->GetWidth() * 0.5f;
    const int digitsEnY = GetHeight() - displayMarginB - mDisplayEnergy->GetHeight();
    mDisplayEnergy->SetPosition(digitsEnX, digitsEnY);

    // icon health
    const int iconHeCenterX = 157;
    const int iconHeX = x0 + iconHeCenterX - mIconHealth->GetWidth() * 0.5f;
    const int iconHeY = y0 + iconsCenterY - mIconHealth->GetHeight() * 0.5f;
    mIconHealth->SetPosition(iconHeX, iconHeY);

    // digits health
    const int digitsHeX = iconHeCenterX - mDisplayHealth->GetWidth() * 0.5f;
    const int digitsHeY = GetHeight() - displayMarginB - mDisplayHealth->GetHeight();
    mDisplayHealth->SetPosition(digitsHeX, digitsHeY);
}

void ButtonQuickUnitSelection::HandleKeyDown(sgl::core::KeyboardEvent & event)
{
    if(event.GetKey() == mShortcutKey)
    {
        HandleButtonDown();

        event.SetConsumed();
    }
}

void ButtonQuickUnitSelection::HandleKeyUp(sgl::core::KeyboardEvent & event)
{
    if(event.GetKey() == mShortcutKey)
    {
        HandleButtonUp();

        event.SetConsumed();
    }
}

void ButtonQuickUnitSelection::HandleButtonUp()
{
    sgl::sgui::ImageButton::HandleButtonUp();

    mScreenGame->CenterCameraOverObject(mUnit);
}

void ButtonQuickUnitSelection::UpdateValues()
{
    const int NUM_TRESH = 4;
    const int tresholds[NUM_TRESH] = { 21, 51, 81, 101 };
    const unsigned int colors[NUM_TRESH] = { 0xf28d8dff, 0xf2c98dff, 0x8de1f2ff, 0x8df295ff };

    // energy
    const int energy = std::roundf(mUnit->GetEnergy() * 100.f / mUnit->GetMaxEnergy());
    mDisplayEnergy->SetValue(energy);

    for(int i = 0; i < NUM_TRESH; ++i)
    {
        if(energy < tresholds[i])
        {
            mDisplayEnergy->SetColorDigits(colors[i]);
            break;
        }
    }

    // health
    const int health = std::roundf(mUnit->GetHealth() * 100.f / mUnit->GetMaxHealth());
    mDisplayHealth->SetValue(health);

    for(int i = 0; i < NUM_TRESH; ++i)
    {
        if(health < tresholds[i])
        {
            mDisplayHealth->SetColorDigits(colors[i]);
            break;
        }
    }
}

} // namespace game
