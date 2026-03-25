#pragma once

#include <sgl/sgui/Widget.h>

#include <string>

namespace sgl  { namespace graphic  { class Renderable; } }

namespace game
{

class DigitsDisplay : public sgl::sgui::Widget
{
public:
    DigitsDisplay(int digits, sgl::sgui::Widget * parent);
    DigitsDisplay(int digits, int fontSize, const std::string & suffix, sgl::sgui::Widget * parent);

    void SetColorZeros(unsigned int color);
    void SetColorDigits(unsigned int color);
    void SetFontSize(int val);
    void SetSuffix(const std::string & suffix);
    void SetValue(int val);

private:
    void UpdateDigits(bool force = false);

    void HandlePositionChanged() override;

    void SetPositions();

private:
    std::string mSuffix;
    int mDigits = 0;
    int mValue = -1;

    int mNumZeros = -1;

    int mFontSize = 19;

    unsigned int mColorZeros = 0x576a75ff;
    unsigned int mColorDigits = 0xE3E6e8FF;

    sgl::graphic::Renderable * mTxtZeros = nullptr;
    sgl::graphic::Renderable * mTxtDigits = nullptr;
};

} // namespace game
