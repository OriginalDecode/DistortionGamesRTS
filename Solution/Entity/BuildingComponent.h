#pragma once
#include "Component.h"
#include <StaticArray.h>

struct BuildingComponentData;

class BuildingComponent : public Component
{
public:
	BuildingComponent(Entity& aEntity, BuildingComponentData& aData);
	~BuildingComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void BuildUnit(eUnitType aUnitType);

	eUnitType GetEntityToSpawn() const;
	const float& GetCurrentBuildTime() const;
	const float& GetMaxBuildTime() const;
	int GetSpawnQueueSize() const;
	int GetUnitCost(eUnitType aUnitType);
	void SetIgnoreBuildTime(bool anIgnoreBuildTime); // enables instabuilding

	bool IsQueueFull() const;

private:

	CU::StaticArray<int, 3> myUnitCosts;
	CU::StaticArray<float, 3> myUnitBuildTimes;

	CU::StaticArray<eUnitType, 5> mySpawnQueue;
	int mySpawnQueueIndex;

	float myCurrentBuildTime;
	float myMaxBuildTime;

	bool myIgnoreBuildTime;
};

inline eComponentType BuildingComponent::GetTypeStatic()
{
	return eComponentType::BUILDING;
}

inline eComponentType BuildingComponent::GetType()
{
	return GetTypeStatic();
}

inline eUnitType BuildingComponent::GetEntityToSpawn() const
{
	return mySpawnQueue[0];
}

inline const float& BuildingComponent::GetCurrentBuildTime() const
{
	return myCurrentBuildTime;
}

inline const float& BuildingComponent::GetMaxBuildTime() const
{
	return myMaxBuildTime;
}

inline int BuildingComponent::GetSpawnQueueSize() const
{
	return mySpawnQueueIndex + 1;
}

inline bool BuildingComponent::IsQueueFull() const
{
	return mySpawnQueueIndex >= 4;
}

inline void BuildingComponent::SetIgnoreBuildTime(bool anIgnoreBuildTime)
{
	myIgnoreBuildTime = anIgnoreBuildTime;
}