#pragma once

#include "Widgets/GameButton.h"

namespace game
{

class ButtonPanelTab : public GameButton
{
public:
    ButtonPanelTab(const char * text, sgl::sgui::Widget * parent);

private:
    void HandleMouseOver() override;

    void HandleButtonDown() override;
};

} // namespace game
