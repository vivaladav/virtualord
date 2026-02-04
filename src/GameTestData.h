#pragma once

namespace game
{

// TEST COMBO BOX
extern const char SpriteFileTestSprite[];
extern const char SpriteFileTestUI[];

enum SpriteIdTestUI : unsigned int
{
    // COMBOBOX MAIN BUTTON BG
    IND_TCB_NORMAL,
    IND_TCB_DISABLED,
    IND_TCB_OVER,
    IND_TCB_PUSHED,
    IND_TCB_CHECKED,
    // COMBOBOX ITEM BUTTON BG
    IND_TCBI_NORMAL,
    IND_TCBI_DISABLED,
    IND_TCBI_OVER,
    IND_TCBI_PUSHED,
    IND_TCBI_CHECKED,
    // SLIDER HORIZ
    IND_TSLIH_BG,
    IND_TSLIH_BAR,
    IND_TSLIH_BUTTON,
    // MULTI SELECTION
    IND_TMULTISEL_BG1,
    IND_TMULTISEL_BG2,
    IND_TMULTISEL_BG3,
    IND_TMULTISEL_BG4,
    IND_TMULTISEL_BG5,
    // CURSORS
    IND_TC_CURSOR1,
    // PROGRESS BAR
    ID_T_PB_BG,
    ID_T_PB_BAR1,
    ID_T_PB_BAR2,
    ID_T_PB_BAR3,
    ID_T_PB_BAR4,
};

constexpr int NUM_TEST_SPRITE_RECTS = 4;

} // namespace game
