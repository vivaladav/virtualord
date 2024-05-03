#include "Tutorial/StepGameConquerCells.h"

#include "IsoMap.h"
#include "Player.h"
#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameConquerCells::StepGameConquerCells(const Player * p, const IsoMap * isoMap)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(550, 350);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 350);

    mInfo->AddInfoEntry("Now that your unit is selected you can decide what cells to conquer.",
                        colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("Moving your mouse around will design a path for your unit to conquer.\n"
                        "You can set anchor points clicking the RIGHT MOUSE BUTTON and finalize "
                        "the path with a double click.", colorTutorialText, 10.f, true, false);
    mInfo->AddInfoEntry("Now move your mouse inside this cell and double click the RIGHT MOUSE BUTTON to "
                        "start the conquest.", colorTutorialTextAction, 0.f, false, false, [this, p, isoMap]
                        {
                            const int destR = 62;
                            const int destC = 13;

                            const sgl::core::Pointd2D pos = isoMap->GetCellPosition(destR, destC);

                            // FOCUS
                            const int marginW = 5;
                            const int marginH = 10;
                            const int objX = pos.x - marginW;
                            const int objY = pos.y - marginH;
                            const int objW = isoMap->GetTileWidth() + (2 * marginW);
                            const int objH = isoMap->GetTileHeight() + (2 * marginH);

                            mFocusArea->SetWorldArea(objX, objY, objW, objH);
                            mFocusArea->SetCornersColor(colorTutorialFocusAction);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            mClickFilter->SetWorldClickableArea(objX, objY, objW, objH);
                            mClickFilter->SetClickableCell(isoMap, destR, destC);

                            mUnit = p->GetUnit(0);
                        });
}

StepGameConquerCells::~StepGameConquerCells()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameConquerCells::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void StepGameConquerCells::Update(float)
{
    if(mUnit != nullptr && mUnit->GetActiveAction() == MOVE)
            SetDone();
}

} // namespace game
