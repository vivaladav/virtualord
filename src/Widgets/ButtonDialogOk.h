#pragma once

#include "Widgets/GameButton.h"

namespace game
{
class ButtonDialogOk : public GameButton
{
public:
    ButtonDialogOk(const char * text, sgl::sgui::Widget * parent);

    void HandleMouseOver() override;

    void HandleButtonDown() override;
};

} // namespace game
