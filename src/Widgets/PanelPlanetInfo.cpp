#include "Widgets/PanelPlanetInfo.h"

#include "GameConstants.h"
#include "MapsRegistry.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <sgl/sgui/Image.h>

#include <sstream>

namespace game
{

PanelPlanetInfo::PanelPlanetInfo()
    : sgl::sgui::Widget(nullptr)
    , mOccupier(NO_FACTION)
    , mStatus(TER_ST_UNKNOWN)
    , mMission(MC_UNKNOWN)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    sm->AddListener(this);

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_INFO);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // TITLE
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFilePanelTitle,
                                      WidgetsConstants::FontSizePlanetMapTitle,
                                      graphic::Font::NORMAL);
    mTitle = new sgui::Label(sm->GetCString("INFO"), fnt, this);
    mTitle->SetColor(WidgetsConstants::colorPanelTitle);

    // -- DATA --
    graphic::Font * fntHeader = fm->GetFont(WidgetsConstants::FontFilePanelHeader,
                                            WidgetsConstants::FontSizePlanetMapHeader,
                                            graphic::Font::NORMAL);
    graphic::Font * fntData = fm->GetFont(WidgetsConstants::FontFileText,
                                          WidgetsConstants::FontSizePlanetMapText,
                                          graphic::Font::NORMAL);

    // LINE SIZE
    mHeaderSize = new sgui::Label(sm->GetCString("SIZE"), fntHeader, this);
    mHeaderSize->SetColor(WidgetsConstants::colorPanelHeader);

    mLabelSize = new sgui::Label("?", fntData, this);
    mLabelSize->SetColor(WidgetsConstants::colorPanelText);

    // LINE STATUS
    mHeaderStatus = new sgui::Label(sm->GetCString("STATUS"), fntHeader, this);
    mHeaderStatus->SetColor(WidgetsConstants::colorPanelHeader);

    mLabelStatus = new sgui::Label("?", fntData, this);
    mLabelStatus->SetColor(WidgetsConstants::colorPanelText);

    // LINE VALUE
    mHeaderValue = new sgui::Label(sm->GetCString("VALUE"), fntHeader, this);
    mHeaderValue->SetColor(WidgetsConstants::colorPanelHeader);

    tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_STARS_0);
    mBarValue = new graphic::Image(tex);
    RegisterRenderable(mBarValue);

    // LINE OCCUPIER
    mHeaderOccupier = new sgui::Label(sm->GetCString("OCCUPIER"), fntHeader, this);
    mHeaderOccupier->SetColor(WidgetsConstants::colorPanelHeader);

    mLabelOccupier = new sgui::Label("?", fntData, this);
    mLabelOccupier->SetColor(WidgetsConstants::colorPanelText);

    // LINE MISSION
    mHeaderMission = new sgui::Label(sm->GetCString("MISSION"), fntHeader, this);
    mHeaderMission->SetColor(WidgetsConstants::colorPanelHeader);

    mLabelMission = new sgui::Label("?", fntData, this);
    mLabelMission->SetColor(WidgetsConstants::colorPanelText);

    // position elements
    UpdatePositions();
}

void PanelPlanetInfo::ClearData()
{
    if(TER_ST_UNKNOWN == mStatus)
        return ;

    mValue = 0;
    mRows = 0;
    mCols = 0;
    mStatus = TER_ST_UNKNOWN;
    mOccupier = NO_FACTION;

    UpdateTerritorySize();
    UpdateTerritoryStatus();
    UpdateTerritoryOccupier();
    UpdateTerritoryValue();

    UpdatePositions();
}

void PanelPlanetInfo::SetData(unsigned int rows, unsigned int cols, TerritoryStatus status,
                              PlayerFaction faction, unsigned int value, MissionCategory mission)
{
    const bool sizeChanged = rows != mRows || cols != mCols;
    const bool statusChanged = status != mStatus;
    const bool factionChanged = faction != mOccupier;
    const bool valueChanged = value != mValue;
    const bool missionChanged = mission != mMission;

    mValue = value;
    mRows = rows;
    mCols = cols;
    mStatus = status;
    mOccupier = faction;
    mMission = mission;

    if(sizeChanged)
        UpdateTerritorySize();

    if(statusChanged)
        UpdateTerritoryStatus();

    if(factionChanged || (NO_FACTION == mOccupier && statusChanged))
        UpdateTerritoryOccupier();

    if(valueChanged)
        UpdateTerritoryValue();

    if(missionChanged)
        UpdateMissionType();

    UpdatePositions();
}

void PanelPlanetInfo::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelPlanetInfo::HandleStateEnabled()
{

}

void PanelPlanetInfo::HandleStateDisabled()
{

}

void PanelPlanetInfo::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = mBg->GetWidth();

    const int marginL = 20;
    const int marginR = 30;
    const int marginT = 15;

    int x = x0;
    int y = y0;

    // BACKGROUND
    mBg->SetPosition(x, y);

    // TITLE
    x = marginL;
    y = marginT;

    mTitle->SetPosition(x, y);

    const int marginTitleB = 15;
    const int marginHeaderB = 5;
    const int marginDataB = 25;

    // LINE SIZE
    x = (w - mHeaderSize->GetWidth()) / 2;
    y += mTitle->GetHeight() + marginTitleB;
    mHeaderSize->SetPosition(x, y);

    x = (w - mLabelSize->GetWidth()) / 2;
    y += mHeaderSize->GetHeight() + marginHeaderB;
    mLabelSize->SetPosition(x, y);

    // LINE STATUS
    x = (w - mHeaderStatus->GetWidth()) / 2;
    y += mLabelSize->GetHeight() + marginDataB;
    mHeaderStatus->SetPosition(x, y);

    x = (w - mLabelStatus->GetWidth()) / 2;
    y += mHeaderStatus->GetHeight() + marginHeaderB;
    mLabelStatus->SetPosition(x, y);

    // LINE VALUE
    x = (w - mHeaderValue->GetWidth()) / 2;
    y += mHeaderStatus->GetHeight() + marginDataB;
    mHeaderValue->SetPosition(x, y);

    x = x0 + (w - mBarValue->GetWidth()) / 2;
    y += y0 + mHeaderValue->GetHeight() + marginHeaderB;
    mBarValue->SetPosition(x, y);

    y -= y0;

    // LINE OCCUPIER
    x = (w - mHeaderOccupier->GetWidth()) / 2;
    y += mBarValue->GetHeight() + marginDataB;
    mHeaderOccupier->SetPosition(x, y);

    x = (w - mLabelOccupier->GetWidth()) / 2;
    y += mHeaderOccupier->GetHeight() + marginHeaderB;
    mLabelOccupier->SetPosition(x, y);

    // LINE MISSION
    x = (w - mHeaderMission->GetWidth()) / 2;
    y += mLabelOccupier->GetHeight() + marginDataB;
    mHeaderMission->SetPosition(x, y);

    x = (w - mLabelMission->GetWidth()) / 2;
    y += mHeaderMission->GetHeight() + marginHeaderB;
    mLabelMission->SetPosition(x, y);
}

void PanelPlanetInfo::UpdateTerritorySize()
{
    if(mRows > 0 && mCols > 0)
    {
        std::ostringstream s;
        s << mRows << "x" << mCols;

        mLabelSize->SetText(s.str().c_str());
    }
    else
        mLabelSize->SetText("?");

    mLabelSize->SetColor(WidgetsConstants::colorPanelText);
}

void PanelPlanetInfo::UpdateTerritoryStatus()
{
    if(mStatus < NUM_TERRITORY_STATUSES)
    {
        auto sm = sgl::utilities::StringManager::Instance();

        const char * statuses[] =
        {
            sm->GetCString("TS_UNEXPLORED"),
            sm->GetCString("TS_FREE"),
            sm->GetCString("TS_OCCUPIED"),
            sm->GetCString("TS_OCCUPIED"),
            sm->GetCString("TS_UNREACHABLE"),
            sm->GetCString("TS_UNAVAILABLE"),
        };

        mLabelStatus->SetText(statuses[mStatus]);
    }
    else
        mLabelStatus->SetText("?");

    mLabelStatus->SetColor(WidgetsConstants::colorPanelText);
}

void PanelPlanetInfo::UpdateTerritoryOccupier()
{
    if(mOccupier < NUM_FACTIONS)
    {
        mLabelOccupier->SetText(FACTIONS_NAME[mOccupier]);
        mLabelOccupier->SetColor(PLAYER_COLOR[mOccupier]);
    }
    else
    {
        if(mStatus != TER_ST_UNKNOWN && mStatus != TER_ST_UNEXPLORED)
            mLabelOccupier->SetText("-");
        else
            mLabelOccupier->SetText("?");

        mLabelOccupier->SetColor(WidgetsConstants::colorPanelText);
    }
}

void PanelPlanetInfo::UpdateTerritoryValue()
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_STARS_0 + mValue);
    mBarValue->SetTexture(tex);
}

void PanelPlanetInfo::UpdateMissionType()
{
    auto sm = sgl::utilities::StringManager::Instance();

    if(mMission < NUM_MISSION_CATEGORIES)
        mLabelMission->SetText(sm->GetCString(MISSION_CATEGORY_TITLE[mMission]));
    else if(MISSION_COMPLETED == mMission)
        mLabelMission->SetText("-");
    // UNKNOWN
    else
        mLabelMission->SetText("?");

    mLabelMission->SetColor(WidgetsConstants::colorPanelText);
}

void PanelPlanetInfo::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mTitle->SetText(sm->GetCString("INFO"));

    mHeaderSize->SetText(sm->GetCString("SIZE"));
    mHeaderStatus->SetText(sm->GetCString("STATUS"));
    mHeaderValue->SetText(sm->GetCString("VALUE"));
    mHeaderOccupier->SetText(sm->GetCString("OCCUPIER"));
    mHeaderMission->SetText(sm->GetCString("MISSION"));

    UpdateTerritoryStatus();
    UpdateMissionType();

    UpdatePositions();
}

} // namespace game
