#include "Tutorial/StepGameCityIntro.h"

#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameCityIntro::StepGameCityIntro(const Cell2D & tl, const Cell2D & br, const IsoMap * isoMap,
                                     const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new IsoFocusArea(isoMap))

{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCellArea(br.row, br.col, tl.row, tl.col);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_CITY_1"), 9.f, true, false,
                        [this]
                        {
                            // FOCUS
                            mFocusArea->SetVisible(true);
                        });

    info->SetFunctionOnFinished([this] { SetDone(); });
}

StepGameCityIntro::~StepGameCityIntro()
{
    delete mFocusArea;
}

} // namespace game
