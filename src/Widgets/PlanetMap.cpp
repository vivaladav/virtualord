#include "Widgets/PlanetMap.h"

#include "GameConstants.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/Point.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/AbstractButtonsGroup.h>

namespace game
{

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
            const int spriteId = IND_PM_CELL_F1 + (mFaction * idPerFaction) +
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
};

// ===== PLANET MAP =====
PlanetMap::PlanetMap()
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = nullptr;

    tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_PLANET);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // CREATE BUTTONS MISSION
    const core::Pointd2D buttonsPos[MAX_MISSIONS] =
    {
        { 199, 94 },
        { 519, 94 },
        { 279, 232 },
        { 439, 232 },
        { 39, 371 },
        { 199, 371 },
        { 519, 371 },
        { 679, 371 },
        { 279, 510 },
        { 439, 510 },
        { 199, 649 },
        { 519, 649 }
    };

    mButtonsMission = new sgui::AbstractButtonsGroup;

    for(int i = 0; i < MAX_MISSIONS; ++i)
    {
        auto btn = new ButtonMission(this);
        btn->SetPosition(buttonsPos[i].x, buttonsPos[i].y);

        mButtonsMission->AddButton(btn);
    }
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
