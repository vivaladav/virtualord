#pragma once

#include <sgl/sgui/Widget.h>

namespace game
{

class IsoMap;

class PanelClickFilter : public sgl::sgui::Widget
{
public:
    PanelClickFilter();

    void SetScreenClickableArea(int x0, int y0, int w, int h);
    void SetWorldClickableArea(int x0, int y0, int w, int h);
    void SetClickableCell(const IsoMap * im, int r, int c);
    void SetClickableCells(const IsoMap * im, int tlR, int tlC, int brR, int brC);

    void ClearButtonToExclude();
    void SetButtonToExclude(int button);

    void ExpandClickableArea();
    void ClearClickableArea();

private:
    void SetClickableArea(int x0, int y0, int w, int h);

    void HandleMouseButtonDown(sgl::core::MouseButtonEvent & event) override;
    void HandleMouseButtonUp(sgl::core::MouseButtonEvent & event) override;

    void FilterMouseEvent(sgl::core::MouseButtonEvent & event);

private:
    int mXtl = 0;
    int mYtl = 0;
    int mXbr = 0;
    int mYbr = 0;

    int mButton;

    const IsoMap * mIsoMap = nullptr;
    int mRow = -1;
    int mCol = -1;
    int mTLR = -1;
    int mTLC = -1;
    int mBRR = -1;
    int mBRC = -1;

    bool mAreaWorld = false;
};

inline void PanelClickFilter::SetButtonToExclude(int b) { mButton = b; }

inline void PanelClickFilter::SetClickableArea(int x0, int y0, int w, int h)
{
    mXtl = x0;
    mYtl = y0;
    mXbr = x0 + w;
    mYbr = y0 + h;
}

inline void PanelClickFilter::SetScreenClickableArea(int x0, int y0, int w, int h)
{
    mAreaWorld = false;

    SetClickableArea(x0, y0, w, h);
}

inline void PanelClickFilter::SetWorldClickableArea(int x0, int y0, int w, int h)
{
    mAreaWorld = true;

    SetClickableArea(x0, y0, w, h);
}

inline void PanelClickFilter::SetClickableCell(const IsoMap * im, int r, int c)
{
    mIsoMap = im;
    mRow = r;
    mCol = c;
}

inline void PanelClickFilter::SetClickableCells(const IsoMap * im, int tlR, int tlC, int brR, int brC)
{
    mIsoMap = im;
    mTLR = tlR;
    mTLC = tlC;
    mBRR = brR;
    mBRC = brC;
}

inline void PanelClickFilter::ClearClickableArea()
{
    SetScreenClickableArea(0, 0, 0, 0);
}

} // namespace game
