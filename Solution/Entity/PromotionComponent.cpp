#include "stdafx.h"

#include <Camera.h>
#include <Engine.h>
#include <ModelLoader.h>
#include "PromotionComponent.h"
#include "PromotionComponentData.h"
#include <SpriteProxy.h>

PromotionComponent::PromotionComponent(Entity& aEntity, PromotionComponentData& aData)
	: Component(aEntity)
	, myKillCount(aData.myKillCount)
{
	CU::Vector2<float> size(32.f, 32.f);
	mySprite = Prism::ModelLoader::GetInstance()->LoadSprite(
		"Data/Resource/Texture/UI/T_unit_promotion.dds", size, size * 0.5f);
	Reset();
}

PromotionComponent::~PromotionComponent()
{
	SAFE_DELETE(mySprite);
}

void PromotionComponent::Reset()
{
	myCurrentKillCount = 0;
}

void PromotionComponent::RenderPromotion(const Prism::Camera& aCamera)
{
	if (GetPromoted() == true)
	{
		CU::Matrix44<float> renderPos;
		renderPos.SetPos(myEntity.GetOrientation().GetPos());
		renderPos = renderPos * CU::InverseSimple(aCamera.GetOrientation());
		renderPos = renderPos * aCamera.GetProjection();

		CU::Vector3<float> newRenderPos = renderPos.GetPos();
		CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

		newRenderPos /= renderPos.GetPos4().w;
		newRenderPos += 1.f;
		newRenderPos *= 0.5f;
		newRenderPos.x *= windowSize.x;
		newRenderPos.y *= windowSize.y;
		newRenderPos.y += 66.f;
		mySprite->Render({ newRenderPos.x, newRenderPos.y });
	}
}