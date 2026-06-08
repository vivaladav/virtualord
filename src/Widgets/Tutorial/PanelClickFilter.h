#pragma once

#include "Cell2D.h"


#include <sgl/sgui/Widget.h>

#include <vector>

namespace game
{

class IsoMap;

class PanelClickFilter : public sgl::sgui::Widget
{
public:
    PanelClickFilter();

    // set 1 cliclable area in screen OR world mode
    void SetScreenClickableArea(int x0, int y0, int w, int h);
    void SetWorldClickableArea(int x0, int y0, int w, int h);
    // set 1 clickable cell OR
    void SetClickableCell(const IsoMap * im, int r, int c);
    void SetClickableCell(const IsoMap * im, const Cell2D & cell);
    // set multiple groups of cell that can be clicked
    void AddClickableCells(const IsoMap * im, int tlR, int tlC, int brR, int brC);
    void AddClickableCells(const IsoMap * im, const Cell2D & tl, const Cell2D & br);

    void ClearButtonToAllow();
    void SetButtonToAllow(int button);

    void ExpandClickableArea();
    void ClearClickableArea();

private:
    void SetClickableArea(int x0, int y0, int w, int h);

    void HandleMouseButtonDown(sgl::core::MouseButtonEvent & event) override;
    void HandleMouseButtonUp(sgl::core::MouseButtonEvent & event) override;

    void FilterMouseEvent(sgl::core::MouseButtonEvent & event);

private:
    struct CellsArea
    {
        CellsArea(int tlR, int tlC, int brR, int brC)
            : rowTL(tlR)
            , colTL(tlC)
            , rowBR(brR)
            , colBR(brC)
        {
        }

        int rowTL;
        int colTL;
        int rowBR;
        int colBR;
    };

    int mXtl = 0;
    int mYtl = 0;
    int mXbr = 0;
    int mYbr = 0;

    int mButton;

    const IsoMap * mIsoMap = nullptr;
    int mRow = -1;
    int mCol = -1;

    std::vector<CellsArea> mCellAreas;

    bool mAreaScreen = false;
    bool mAreaWorld = false;
};

inline void PanelClickFilter::SetButtonToAllow(int b) { mButton = b; }

inline void PanelClickFilter::SetClickableArea(int x0, int y0, int w, int h)
{
    mXtl = x0;
    mYtl = y0;
    mXbr = x0 + w;
    mYbr = y0 + h;
}

inline void PanelClickFilter::SetScreenClickableArea(int x0, int y0, int w, int h)
{
    mAreaScreen = true;
    mAreaWorld = false;

    SetClickableArea(x0, y0, w, h);
}

inline void PanelClickFilter::SetWorldClickableArea(int x0, int y0, int w, int h)
{
    mAreaScreen = false;
    mAreaWorld = true;

    SetClickableArea(x0, y0, w, h);
}

inline void PanelClickFilter::SetClickableCell(const IsoMap * im, int r, int c)
{
    mIsoMap = im;
    mRow = r;
    mCol = c;
}

inline void PanelClickFilter::SetClickableCell(const IsoMap * im, const Cell2D & cell)
{
    mIsoMap = im;
    mRow = cell.row;
    mCol = cell.col;
}

inline void PanelClickFilter::AddClickableCells(const IsoMap * im, int tlR, int tlC, int brR, int brC)
{
    mIsoMap = im;
    mCellAreas.emplace_back(tlR, tlC, brR, brC);
}

inline void PanelClickFilter::AddClickableCells(const IsoMap * im, const Cell2D & tl, const Cell2D & br)
{
    mIsoMap = im;
    mCellAreas.emplace_back(tl.row, tl.col, br.row, br.col);
}

inline void PanelClickFilter::ClearClickableArea()
{
    SetScreenClickableArea(0, 0, 0, 0);
}

} // namespace game
