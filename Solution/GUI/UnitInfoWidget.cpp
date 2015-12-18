#include "stdafx.h"
#include "BarWidget.h"
#include <Engine.h>
#include "../Entity/BuildingComponent.h"
#include "../Entity/Entity.h"
#include "../Game/PlayerDirector.h"
#include "../Entity/ActorComponent.h"
#include "../Entity/ControllerComponent.h"
#include "../Entity/HealthComponent.h"
#include "UnitInfoWidget.h"
#include "WidgetContainer.h"

namespace GUI
{
	UnitInfoWidget::UnitInfoWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: myUnits(aPlayer->GetSelectedUnits())
		, mySelectedType(eEntityType::_COUNT)
		, myBuilding(aPlayer->GetBuildingComponent())
		, myBuildingTimer(nullptr)
		, myTextScale(1.f)
	{
		std::string gruntUnitPath = "";
		std::string gruntPortraitPath = "";
		std::string buildingPortraitPath = "";
		std::string statsSpritePath = "";
		CU::Vector2<float> size;
		CU::Vector2<float> unitSize;
		CU::Vector2<float> portraitSize;
		CU::Vector2<float> portraitPosition;
		CU::Vector2<float> unitPosition;
		CU::Vector2<float> statsSize;
		
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "x", unitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitsize"), "y", unitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "x", portraitSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitsize"), "y", portraitSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssize"), "x", statsSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssize"), "y", statsSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "x", unitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "unitposition"), "y", unitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "x", portraitPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "portraitposition"), "y", portraitPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntunit"), "path", gruntUnitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "gruntportrait"), "path", gruntPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buildingportrait"), "path", buildingPortraitPath);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "statssprite"), "path", statsSpritePath);

		mySize = size;
		myPosition = { 0.f, 0.f };
		myUnitPosition = unitPosition;
		myPortraitPosition = portraitPosition;
		myGruntUnit = new Prism::Sprite(gruntUnitPath, unitSize);
		myGruntPortrait = new Prism::Sprite(gruntPortraitPath, portraitSize);
		myBuildingPortrait = new Prism::Sprite(buildingPortraitPath, portraitSize);
		myStatsSprite = new Prism::Sprite(statsSpritePath, statsSize);
		myBuildingTimer = new BarWidget(myBuilding.GetMaxBuildTime(), myBuilding.GetCurrentBuildTime(), { unitSize.x * 4.f, unitSize.y / 2.f });
	}

	UnitInfoWidget::~UnitInfoWidget()
	{
		SAFE_DELETE(myGruntPortrait);
		SAFE_DELETE(myGruntUnit);
		SAFE_DELETE(myBuildingPortrait);
		SAFE_DELETE(myStatsSprite);
		SAFE_DELETE(myBuildingTimer);
	}

	void UnitInfoWidget::Update()
	{
		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}
	}

	void UnitInfoWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				if (myUnits.Size() > 1)
				{
					for (int i = 0; i < myUnits.Size(); i++)
					{
						CU::Vector2<float> position = { myPosition.x + myGruntUnit->GetSize().x * i, myPosition.y };
						position += aParentPosition + myUnitPosition;
						myGruntUnit->Render(position);
					}
				}
				else
				{
					RenderUnitInfo(aParentPosition);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				RenderBaseInfo(aParentPosition);
			}
		}
	}

	void UnitInfoWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		myBuildingTimer->OnResize(aNewWindowSize, anOldWindowSize);

		CU::Vector2<float> unitRatioSize = myGruntUnit->GetSize() / anOldWindowSize;
		CU::Vector2<float> portraitRatioSize = myBuildingPortrait->GetSize() / anOldWindowSize;
		CU::Vector2<float> ratioUnitPostion = myUnitPosition / anOldWindowSize;
		CU::Vector2<float> ratioPortraitPostion = myPortraitPosition / anOldWindowSize;
		CU::Vector2<float> ratioStatsSize = myStatsSprite->GetSize() / anOldWindowSize;

		myUnitPosition = ratioUnitPostion * aNewWindowSize;
		myPortraitPosition = ratioPortraitPostion * aNewWindowSize;
		myGruntUnit->SetSize(unitRatioSize * aNewWindowSize, { 0.f, 0.f });
		myGruntPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
		myBuildingPortrait->SetSize(portraitRatioSize * aNewWindowSize, { 0.f, 0.f });
		myStatsSprite->SetSize(ratioStatsSize * aNewWindowSize, { 0.f, 0.f });
		//myTextScale = myTextScale /;
	}

	void UnitInfoWidget::RenderBaseInfo(const CU::Vector2<float>& aParentPosition)
	{
		myBuildingPortrait->Render(myPosition + aParentPosition + myPortraitPosition);

		if (myBuilding.GetEntityToSpawn() != eEntityType::EMPTY)
		{
			myBuildingTimer->Update();
			CU::Vector2<float> position = myPosition + aParentPosition + myUnitPosition + (myBuildingTimer->GetSize() / 2.f);
			myBuildingTimer->Render(position);
			position.x += myBuildingTimer->GetSize().x + 10.f;
			Prism::Engine::GetInstance()->PrintText(myBuilding.GetSpawnQueueSize(), position, Prism::eTextType::RELEASE_TEXT, myTextScale);
		}
	}

	void UnitInfoWidget::RenderUnitInfo(const CU::Vector2<float>& aParentPosition)
	{
		CU::Vector2<float> portraitPosition = myPosition + aParentPosition + myPortraitPosition;
		myGruntPortrait->Render(portraitPosition);
		portraitPosition.y -= myGruntPortrait->GetSize().y / 3.5f;
		portraitPosition.x += myGruntPortrait->GetSize().x / 3.f;

		std::string health = std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetCurrentHealth())) 
			+ "/" + std::to_string(int(myUnits[0]->GetComponent<HealthComponent>()->GetMaxHealth()));
		Prism::Engine::GetInstance()->PrintText(health, portraitPosition, Prism::eTextType::RELEASE_TEXT, myTextScale);

		CU::Vector2<float> position = { myGruntUnit->GetSize().x / 2.f, myPosition.y };
		position += aParentPosition + myUnitPosition;
		myStatsSprite->Render(position);

		position.x += myStatsSprite->GetSize().x / 20.f;
		position.y -= myStatsSprite->GetSize().y / 2.f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<HealthComponent>()->GetArmor()
			, position, Prism::eTextType::RELEASE_TEXT, myTextScale);

		position.x += myStatsSprite->GetSize().x / 3.f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetComponent<ControllerComponent>()->GetAttackDamage()
			, position, Prism::eTextType::RELEASE_TEXT, myTextScale);

		position.x += myStatsSprite->GetSize().x / 3.f;
		Prism::Engine::GetInstance()->PrintText(myUnits[0]->GetMaxSpeed()
			, position, Prism::eTextType::RELEASE_TEXT, myTextScale);
	}
}