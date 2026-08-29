#pragma once

#include "IsoObject.h"

namespace game
{

class IndicatorSelection : public IsoObject
{
public:
    enum IndicatorType : unsigned int
    {
        SEL_IND_CELL,
        SEL_IND_TL,
        SEL_IND_BR,
        SEL_IND_BL,
        SEL_IND_TR,

        NUM_INDICATOR_SELECTION_TYPES,

        SEL_IND_NULL,
    };

public:
    IndicatorSelection(IndicatorType type, bool primary);

    void SetType(IndicatorType type);

    void SetPrimary(bool val);

private:
    IndicatorType mType = SEL_IND_NULL;

    bool mPrimary;
};

} // namespace game
