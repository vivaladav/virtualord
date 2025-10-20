#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }

    namespace sgui { class Label; }
}

namespace game
{

enum MissionCategory : unsigned int;
enum PlayerFaction : unsigned int;
enum TerritoryStatus : unsigned int;

class PanelPlanetInfo : public sgl::sgui::Widget
{
public:
    PanelPlanetInfo();

    void ClearData();
    void SetData(int size, TerritoryStatus status, PlayerFaction faction,
                 unsigned int value, MissionCategory mission);

private:
    void HandlePositionChanged() override;

    void HandleStateEnabled() override;
    void HandleStateDisabled() override;

    void UpdatePositions();

    void UpdateTerritorySize();
    void UpdateTerritoryStatus();
    void UpdateTerritoryOccupier();
    void UpdateTerritoryValue();
    void UpdateMissionType();

private:
    sgl::graphic::Text * mTitle = nullptr;
    sgl::graphic::Text * mHeaderSize = nullptr;
    sgl::graphic::Text * mHeaderStatus = nullptr;
    sgl::graphic::Text * mHeaderValue = nullptr;
    sgl::graphic::Text * mHeaderOccupier = nullptr;
    sgl::graphic::Text * mHeaderMission = nullptr;

    sgl::graphic::Text * mLabelSize = nullptr;
    sgl::graphic::Text * mLabelStatus = nullptr;
    sgl::graphic::Text * mLabelOccupier = nullptr;
    sgl::graphic::Text * mLabelMission = nullptr;
    sgl::graphic::Image * mBarValue = nullptr;

    sgl::graphic::Image * mBg = nullptr;

    int mSize = 0;
    int mValue = 0;
    PlayerFaction mOccupier;
    TerritoryStatus mStatus;
    MissionCategory mMission;
};

} // namespace game
