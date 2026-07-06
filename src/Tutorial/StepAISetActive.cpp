#include "Tutorial/StepAISetActive.h"

#include "AI/PlayerAI.h"

namespace game
{

StepAISetActive::StepAISetActive(PlayerAI * ai, bool active)
    : mAI(ai)
    , mActive(active)
{
}

void StepAISetActive::OnStart()
{
    mAI->SetActive(mActive);

    SetDone();
}

} // namespace game
