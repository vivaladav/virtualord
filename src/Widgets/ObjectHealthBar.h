#pragma once

#include <sgl/sgui/Image.h>

namespace game
{

enum PlayerFaction : unsigned int;

class ObjectHealthBar : public sgl::sgui::Image
{
public:
    ObjectHealthBar(unsigned int val);

    void SetValue(unsigned int val);

public:
    static const unsigned int MAX_VAL = 10;

private:
    void UpdateImage();

private:
    unsigned int mValue = -1;
};

} // namespace game
