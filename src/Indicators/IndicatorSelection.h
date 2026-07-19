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
        SEL_IND_TR,
        SEL_IND_BL,
        SEL_IND_BR,

        NUM_INDICATOR_SELECTION_TYPES,

        SEL_IND_NULL,
    };

public:
    IndicatorSelection(IndicatorType type);

    void SetType(IndicatorType type);

private:

private:
    IndicatorType mType = SEL_IND_NULL;
};

} // namespace game
