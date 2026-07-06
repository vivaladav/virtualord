#include "ButtonUnitsSelector.h"

#include "GameTestData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>

namespace game
{

ButtonUnitsSelector::ButtonUnitsSelector(const char * text)
    : GameButton( SpriteFileTestUI,
                 {
                    IND_TMULTISEL_BG1,
                    IND_TMULTISEL_BG2,
                    IND_TMULTISEL_BG3,
                    IND_TMULTISEL_BG4,
                    IND_TMULTISEL_BG5
                 },
                 {
                     0xCCCCCCFF,
                     0x777777ff,
                     0xFFFFFFFF,
                     0x33FFFFFF,
                     0xFFFF33FF
                 },
                 nullptr)
{
    using namespace sgl::graphic;

    // set label font
    auto fm = FontManager::Instance();
    Font * font = fm->GetFont(WidgetsConstants::FontFileButton, 24, Font::NORMAL);

    SetLabelFont(font);

    // init label
    SetLabel(text);
}

} // namespace game
