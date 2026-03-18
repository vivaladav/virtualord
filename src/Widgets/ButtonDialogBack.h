#pragma once

#include <sgl/sgui/ImageButton.h>

namespace game
{

class ButtonDialogBack : public sgl::sgui::ImageButton
{
public:
    ButtonDialogBack(sgl::sgui::Widget * parent = nullptr);

private:
    void HandleMouseOver() override;
    void HandleButtonDown() override;
};

} // namespace game
