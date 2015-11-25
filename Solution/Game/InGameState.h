#pragma once
#include "GameState.h"
#include <Matrix.h>
#include <GrowingArray.h>
#include <Subscriber.h>

class Level;

namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class Camera;
	class Sprite;
	class ParticleEmitterInstance;
}

class InGameState : public GameState, public Subscriber
{
public:

	InGameState(CU::InputWrapper* anInputWrapper);
	~InGameState();

	void InitState(StateStackProxy* aStateStackProxy) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const GameStateMessage& aMessage) override;

	void SetLevel();

	void CompleteLevel();
	void CompleteGame();
	void LoadLevelSettings(); 
	void LoadPlayerSettings();

	bool IsComplete() const;

private:
	void UpdateCamera(float aDeltaTime);
	void ShowMessage(const std::string& aBackgroundPath, const CU::Vector2<float>& aSize, std::string aText, GameStateMessage* aMessage = nullptr);

	Level* myLevel;
	Prism::Camera* myCamera;
	Prism::ParticleEmitterInstance* myEmitter;
	CU::Matrix44<float> myCameraOrientation;

	bool myIsComplete;
};

inline bool InGameState::IsComplete() const
{
	return myIsComplete;
}