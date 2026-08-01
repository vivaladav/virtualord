#include "Widgets/DisappearingLabel.h"

namespace game
{

DisappearingLabel::DisappearingLabel(sgl::graphic::Font * font, const char * text, float timeSec)
    : sgl::sgui::Label(text, font)
    , mTimer(timeSec)
{
}

void DisappearingLabel::OnUpdate(float delta)
{
    if(mTimer > 0.f)
        mTimer -= delta;
    else
        DeleteLater();
}

} // namespace game
