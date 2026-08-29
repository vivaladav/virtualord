#pragma once

#include <unordered_map>
#include <vector>

namespace game
{

class GameObject;
class IndicatorSelection;
class IsoLayer;
class IsoMap;

class OverlaySelection
{
public:
    OverlaySelection(IsoLayer * layer);
    ~OverlaySelection();

    void ClearIndicators();
    void AddObject(GameObject * obj, bool primary = false);

    void UpdateObjectPosition(GameObject * obj);
    void UpdateObjectCell(GameObject * obj);

private:
    IndicatorSelection * GetNewIndicator(unsigned int type, bool primary);

private:
    struct MultiIndicator
    {
        MultiIndicator(IndicatorSelection * tl, IndicatorSelection * tr, IndicatorSelection * bl, IndicatorSelection * br)
            : indTL(tl)
            , indTR(tr)
            , indBL(bl)
            , indBR(br)
        {
        }

        IndicatorSelection * indTL = nullptr;
        IndicatorSelection * indTR = nullptr;
        IndicatorSelection * indBL = nullptr;
        IndicatorSelection * indBR = nullptr;
    };

    std::vector<IndicatorSelection *> mActiveIndicators;
    std::vector<IndicatorSelection *> mAvailableIndicators;

    std::unordered_map<GameObject *, IndicatorSelection *> mCellIndicators;
    std::unordered_map<GameObject *, MultiIndicator> mAreaIndicators;

    IsoLayer * mLayer = nullptr;
};

} // namespace game
