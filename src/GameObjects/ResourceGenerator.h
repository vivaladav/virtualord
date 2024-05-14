#pragma once

#include "Structure.h"

namespace game
{

enum ResourceType : unsigned int;

class ResourceGenerator : public Structure
{
public:
    ResourceGenerator(GameObjectTypeId type, int rows, int cols);

    int GetOutput() const;
    void ScaleOutput(float mult);

    ResourceType GetResourceType() const;

    void OnNewTurn(PlayerFaction faction) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void UpdateOutput();

private:
    ResourceType mResource;

    int mOutput = 0;
};

inline int ResourceGenerator::GetOutput() const { return mOutput; }

inline ResourceType ResourceGenerator::GetResourceType() const { return mResource; }

} // namespace game

