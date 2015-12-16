#pragma once

namespace Prism
{
	class Terrain;
}

struct ControllerComponentData;
class ControllerComponent : public Component
{
public:
	struct ControllerData
	{
		CU::Vector3<float> myDirection;
	};

	ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain);
	~ControllerComponent();

	void Update(float aDelta) override;

	void MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue);
	void AttackTarget(Entity* aEntity, bool aClearCommandQueue);
	void AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue);
	void Stop();
	void HoldPosition();

	void Spawn(const CU::Vector3f& aPosition);

	const ControllerData& GetControllerData() const;
	float GetVisionRange2() const;
	float GetAttackRange2() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	float GetAttackDamage() const;

private:
	enum class eAction
	{
		IDLE,
		MOVE,
		ATTACK_TARGET,
		RETURN,
		HOLD_POSITION,
		ATTACK_MOVE
	};

	struct ActionData
	{
		eAction myAction;
		CU::Vector3<float> myPosition;
	};

	void FillCommandList(const CU::Vector3<float>& aTargetPosition, eAction aAction, bool aClearCommandQueue);
	void DoMoveAction(const CU::Vector3<float>& aTargetPosition);
	void DoAttackAction();
	void AttackTarget();
	void StartNextAction();
	void RefreshPathToAttackTarget();
	void RenderDebugLines() const;
	eColorDebug GetActionColor(eAction aAction) const;
	
	Entity* myAttackTarget;
	ControllerData myData;

	CU::Vector3<float> myReturnPosition;

	const Prism::Terrain& myTerrain;
	eOwnerType myOwnerType;
	int myTargetType;

	float myVisionRange2;
	float myAttackRange2;
	float myAttackDamage;
	float myAttackSpeed;
	float myAttackTimer;
	float myChaseDistance2;
	float myChaseDistanceNeutral2;
	float myAttackTargetPathRefreshTime;
	float myCurrentAttackTargetPathRefreshTime;

	CU::GrowingArray<ActionData> myActions;
	ActionData myCurrentAction;
};

inline const ControllerComponent::ControllerData& ControllerComponent::GetControllerData() const
{
	return myData;
}

inline float ControllerComponent::GetVisionRange2() const
{
	return myVisionRange2;
}

inline float ControllerComponent::GetAttackRange2() const
{
	return myAttackRange2;
}

inline eComponentType ControllerComponent::GetTypeStatic()
{
	return eComponentType::CONTROLLER;
}

inline eComponentType ControllerComponent::GetType()
{
	return GetTypeStatic();
}

inline float ControllerComponent::GetAttackDamage() const
{
	return myAttackDamage;
}