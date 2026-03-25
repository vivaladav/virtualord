#include "Widgets/PlanetMap.h"

#include "GameConstants.h"
#include "MapsRegistry.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/Point.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/AbstractButtonsGroup.h>

#include <cassert>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ===== BUTTON MISSION =====
class ButtonMission : public sgl::sgui::AbstractButton
{
public:
    ButtonMission(sgl::sgui::Widget * parent)
        : sgl::sgui::AbstractButton(parent)
    {
        mBody = new sgl::graphic::Image;
        RegisterRenderable(mBody);

        SetCheckable(true);

        InitState(NORMAL);
        UpdateGraphics(NORMAL);
    }

    void SetMain(bool main)
    {
        if(main != mMain)
        {
            mMain = main;
            UpdateGraphics(GetState());
        }
    }

    void SetFaction(PlayerFaction faction)
    {
        if(faction == mFaction)
            return ;

        mFaction = faction;

        UpdateGraphics(GetState());
    }

    void SetTerritoryStatus(TerritoryStatus ts)
    {
        if(ts == mTerritoryStatus)
            return ;

        mTerritoryStatus = ts;

        UpdateGraphics(GetState());
    }

    bool IsScreenPointInside(int x, int y) override
    {
        const int border = 2;
        const int hw = (GetWidth() * 0.5f) - border;
        const int hh = (GetHeight() * 0.5f) - border;
        const int radius2 = hw > hh ? hw * hw : hh * hh;

        const int xc = GetScreenX() + hw;
        const int yc = GetScreenY() + hh;

        const int dx = xc - x;
        const int dy = yc - y;
        const int dist2 = (dx * dx) + (dy * dy);

        return dist2 < radius2;
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        if(IsChecked())
            return ;

        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }

    void OnStateChanged(VisualState state) override { UpdateGraphics(state); }

    void UpdateGraphics(VisualState state)
    {
        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = nullptr;

        // occupation known -> show it
        if(TER_ST_OCCUPIED == mTerritoryStatus || TER_ST_OCCUPIED_UNEXPLORED == mTerritoryStatus)
        {
            const int idPerFaction = 2;
            const int spriteId0 = mMain ? IND_PM_MAIN_CELL_F1 : IND_PM_CELL_F1;
            const int spriteId = spriteId0 + (mFaction * idPerFaction) +
                                 static_cast<int>(IsChecked());

            tex = tm->GetSprite(SpriteFilePlanetMap, spriteId);
        }
        // cell unexplored
        else if(TER_ST_UNEXPLORED == mTerritoryStatus)
        {
            const unsigned int texId[NUM_VISUAL_STATES] =
            {
                IND_PM_CELL_UNEXPLORED,
                IND_PM_CELL_DISABLED,
                IND_PM_CELL_UNEXPLORED_SEL,
                IND_PM_CELL_UNEXPLORED_SEL,
                IND_PM_CELL_UNEXPLORED_SEL
            };

            const unsigned int texIdM[NUM_VISUAL_STATES] =
            {
                IND_PM_MAIN_CELL_UNEXPLORED,
                IND_PM_MAIN_CELL_DISABLED,
                IND_PM_MAIN_CELL_UNEXPLORED_SEL,
                IND_PM_MAIN_CELL_UNEXPLORED_SEL,
                IND_PM_MAIN_CELL_UNEXPLORED_SEL
            };

            if(mMain)
                tex = tm->GetSprite(SpriteFilePlanetMap, texIdM[state]);
            else
                tex = tm->GetSprite(SpriteFilePlanetMap, texId[state]);
        }
        // explored and free or unrechable (which will be disabled)
        else
        {
            const unsigned int texId[NUM_VISUAL_STATES] =
            {
                IND_PM_CELL_EXPLORED,
                IND_PM_CELL_DISABLED,
                IND_PM_CELL_SELECTED,
                IND_PM_CELL_SELECTED,
                IND_PM_CELL_SELECTED
            };

            const unsigned int texIdM[NUM_VISUAL_STATES] =
            {
                IND_PM_MAIN_CELL_EXPLORED,
                IND_PM_MAIN_CELL_DISABLED,
                IND_PM_MAIN_CELL_SELECTED,
                IND_PM_MAIN_CELL_SELECTED,
                IND_PM_MAIN_CELL_SELECTED
            };

            if(mMain)
                tex = tm->GetSprite(SpriteFilePlanetMap, texIdM[state]);
            else
                tex = tm->GetSprite(SpriteFilePlanetMap, texId[state]);
        }

        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());
    }

    void HandlePositionChanged() override
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        mBody->SetPosition(x0, y0);
    }

private:
     sgl::graphic::Image * mBody = nullptr;

     PlayerFaction mFaction = NO_FACTION;
     TerritoryStatus mTerritoryStatus = TER_ST_UNKNOWN;

     bool mMain = false;
};

} // namespace

namespace game
{

// ===== PLANET MAP =====
PlanetMap::PlanetMap(PlanetSize ps)
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = nullptr;

    tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_PLANET);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int dimS = 500;
    const int dimM = 600;

    if(ps == PLANET_SIZE_S)
        mBg->SetSize(dimS, dimS);
    else if(ps == PLANET_SIZE_M)
        mBg->SetSize(dimM, dimM);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // CREATE BUTTONS MISSION
    std::vector<core::Pointd2D> buttonsPos;

    if(ps == PLANET_SIZE_S)
    {
        buttonsPos.emplace_back(214, 34);
        buttonsPos.emplace_back(214, 404);
        buttonsPos.emplace_back(71, 219);
        buttonsPos.emplace_back(356, 219);
        buttonsPos.emplace_back(202, 209);
    }
    else if(ps == PLANET_SIZE_M)
    {
        buttonsPos.emplace_back(93, 121);
        buttonsPos.emplace_back(435, 121);
        buttonsPos.emplace_back(93, 417);
        buttonsPos.emplace_back(435, 417);
        buttonsPos.emplace_back(93, 269);
        buttonsPos.emplace_back(435, 269);
        buttonsPos.emplace_back(252, 259);
    }
    else
    {
        buttonsPos.emplace_back(314, 60);
        buttonsPos.emplace_back(314, 578);
        buttonsPos.emplace_back(115, 146);
        buttonsPos.emplace_back(513, 146);
        buttonsPos.emplace_back(115, 492);
        buttonsPos.emplace_back(513, 492);
        buttonsPos.emplace_back(115, 319);
        buttonsPos.emplace_back(513, 319);
        buttonsPos.emplace_back(302, 309);
    }

    const unsigned int numMissions = buttonsPos.size();
    mButtonsMission = new sgui::AbstractButtonsGroup;
    ButtonMission * mainBtn = nullptr;

    for(int i = 0; i < numMissions; ++i)
    {
        auto btn = new ButtonMission(this);
        btn->SetPosition(buttonsPos[i].x, buttonsPos[i].y);

        mButtonsMission->AddButton(btn);

        mainBtn = btn;
    }

    assert(mainBtn != nullptr);

    mainBtn->SetMain(true);
}

PlanetMap::~PlanetMap()
{
    delete mButtonsMission;
}

void PlanetMap::SetFunctionOnToggle(const std::function<void(unsigned int, bool)> & f)
{
    mButtonsMission->SetFunctionOnToggle(f);
}

void PlanetMap::SetButtonEnabled(unsigned int index, bool enabled)
{
    if(index >= mButtonsMission->GetNumButtons())
        return ;

    auto b = static_cast<ButtonMission *>(mButtonsMission->GetButton(index));
    b->SetEnabled(enabled);
}

void PlanetMap::SetButtonState(unsigned int index, PlayerFaction occupier, TerritoryStatus ts)
{
    if(index >= mButtonsMission->GetNumButtons())
        return ;

    auto b = static_cast<ButtonMission *>(mButtonsMission->GetButton(index));

    if(ts == TER_ST_UNAVAILABLE)
    {
        b->SetVisible(false);
        return;
    }

    const bool enabled = ts != TER_ST_UNREACHABLE;

    b->SetFaction(occupier);
    b->SetTerritoryStatus(ts);
    b->SetEnabled(enabled);
}

int PlanetMap::GetSelectedTerritoryId() const
{
    return mButtonsMission->GetIndexChecked();
}

sgl::sgui::AbstractButton * PlanetMap::GetButton(unsigned int ind) const
{
    return mButtonsMission->GetButton(ind);
}

void PlanetMap::HandlePositionChanged()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    mBg->SetPosition(x0, y0);
}

} // namespace game
