#include "stdafx.h"
#include "AbilityButton.h"
#include "ButtonWidget.h"
#include "../Entity/Entity.h"
#include "../Entity/EnrageComponent.h"
#include "../Entity/GrenadeComponent.h"
#include "../Entity/PromotionComponent.h"
#include "../Game/PlayerDirector.h"
#include "UnitActionWidget.h"
#include "UpgradeButtonWidget.h"
#include "WidgetContainer.h"
#include "../Entity/ActorComponent.h"

namespace GUI
{
	UnitActionWidget::UnitActionWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const CU::GrowingArray<Entity*>& someUnits
		, const PlayerDirector* aPlayer)
		: Widget()
		, myUnits(someUnits)
		, myUnitActionButtons(nullptr)
		, myBuildingActionButtons(nullptr)
		, myIsUnitSelected(false)
		, mySelectedType(eEntityType::_COUNT)
		, myPlayer(aPlayer)
		, myHasUnlockedRanger(aPlayer->HasUnlockedRanger())
		, myHasUnlockedTank(aPlayer->HasUnlockedTank())
		, myFirstSelectedGrunt(nullptr)
		, myFirstSelectedRanger(nullptr)
		, myFirstSelectedTank(nullptr)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;
		CU::Vector2<float> buttonSize;
		CU::Vector2<float> buttonPosition;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		mySize = size;
		myPosition = position;

		tinyxml2::XMLElement* unitElement = aReader->ForceFindFirstChild(anXMLElement, "unit");
		tinyxml2::XMLElement* gruntElement = aReader->ForceFindFirstChild(anXMLElement, "grunt");
		tinyxml2::XMLElement* rangerElement = aReader->ForceFindFirstChild(anXMLElement, "ranger");
		tinyxml2::XMLElement* tankElement = aReader->ForceFindFirstChild(anXMLElement, "tank");
		tinyxml2::XMLElement* buildingElement = aReader->ForceFindFirstChild(anXMLElement, "building");

		myUnitActionButtons = ReadContainer(aReader, unitElement, size);
		myBuildingActionButtons = ReadContainer(aReader, buildingElement, size);

		buttonSize = myUnitActionButtons->GetFirstWidget()->GetSize();
		buttonPosition = myUnitActionButtons->GetFirstWidget()->GetPosition();

		buttonPosition.x -= (buttonSize.x * 0.5f);
		buttonPosition.y += (buttonSize.y * 0.5f);

		myGruntActionButtons = ReadContainer(aReader, gruntElement, buttonSize);
		myRangerActionButtons = ReadContainer(aReader, rangerElement, buttonSize);
		myTankActionButtons = ReadContainer(aReader, tankElement, buttonSize);

		myGruntActionButtons->SetPosition(myGruntActionButtons->At(0)->GetPosition() - myGruntActionButtons->At(0)->GetSize() / 2.f);
		myGruntActionButtons->At(0)->SetPosition(myGruntActionButtons->At(0)->GetSize() / 2.f, false);
		myGruntActionButtons->At(1)->SetPosition(myGruntActionButtons->At(0)->GetPosition(), false);
		
		myRangerActionButtons->SetPosition(myRangerActionButtons->At(0)->GetPosition() - myRangerActionButtons->At(0)->GetSize() / 2.f);
		myRangerActionButtons->At(0)->SetPosition(myRangerActionButtons->At(0)->GetSize() / 2.f, false);
		myRangerActionButtons->At(1)->SetPosition(myRangerActionButtons->At(0)->GetPosition(), false);

		myTankActionButtons->SetPosition(myTankActionButtons->At(0)->GetPosition() - myTankActionButtons->At(0)->GetSize() / 2.f);
		myTankActionButtons->At(0)->SetPosition(myTankActionButtons->At(0)->GetSize() / 2.f, false);
		myTankActionButtons->At(1)->SetPosition(myTankActionButtons->At(0)->GetPosition(), false);
	}

	UnitActionWidget::~UnitActionWidget()
	{
		SAFE_DELETE(myUnitActionButtons);
		SAFE_DELETE(myGruntActionButtons);
		SAFE_DELETE(myRangerActionButtons);
		SAFE_DELETE(myTankActionButtons);
		SAFE_DELETE(myBuildingActionButtons);
	}

	void UnitActionWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				myUnitActionButtons->Render(myPosition + aParentPosition);

				if (myFirstSelectedGrunt != nullptr)
				{
					myFirstSelectedGrunt->GetComponent<PromotionComponent>()->GetPromoted() ? 
						myGruntActionButtons->Render(myPosition + aParentPosition, 0) : myGruntActionButtons->Render(myPosition + aParentPosition, 1);
				}

				if (myFirstSelectedRanger != nullptr)
				{
					myFirstSelectedRanger->GetComponent<PromotionComponent>()->GetPromoted() ?
						myRangerActionButtons->Render(myPosition + aParentPosition, 0) : myRangerActionButtons->Render(myPosition + aParentPosition, 1);
				}

				if (myFirstSelectedTank != nullptr)
				{
					myFirstSelectedTank->GetComponent<PromotionComponent>()->GetPromoted() ?
						myTankActionButtons->Render(myPosition + aParentPosition, 0) : myTankActionButtons->Render(myPosition + aParentPosition, 1);
				}
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				myBuildingActionButtons->At(0)->Render(myPosition + aParentPosition);
				myBuildingActionButtons->At(5)->Render(myPosition + aParentPosition);

				if (myHasUnlockedRanger == true)
				{
					myBuildingActionButtons->At(1)->Render(myPosition + aParentPosition);
					myBuildingActionButtons->At(6)->Render(myPosition + aParentPosition);

				}
				else
				{
					myBuildingActionButtons->At(2)->Render(myPosition + aParentPosition);
					myBuildingActionButtons->At(8)->Render(myPosition + aParentPosition);
				}

				if (myHasUnlockedTank == true)
				{
					myBuildingActionButtons->At(3)->Render(myPosition + aParentPosition);
					myBuildingActionButtons->At(7)->Render(myPosition + aParentPosition);
				}
				else
				{
					myBuildingActionButtons->At(4)->Render(myPosition + aParentPosition);
					myBuildingActionButtons->At(9)->Render(myPosition + aParentPosition);
				}
			}
		}
	}

	void UnitActionWidget::Update(float)
	{
		myFirstSelectedGrunt = nullptr;
		myFirstSelectedRanger = nullptr;
		myFirstSelectedTank = nullptr;

		myIsUnitSelected = myUnits.Size() > 0;
		if (myIsUnitSelected == true)
		{
			mySelectedType = myUnits[0]->GetType();
		}

		if (mySelectedType == eEntityType::UNIT)
		{
			for (int i = 0; i < myUnits.Size(); i++)
			{
				if (myFirstSelectedGrunt == nullptr && myUnits[i]->GetUnitType() == eUnitType::GRUNT)
				{
					myGruntActionButtons->At(0)->SetValue(myUnits[i]->GetComponent<GrenadeComponent>()->GetCooldown(), myUnits[i]->GetComponent<GrenadeComponent>()->GetMaxCooldown());
					myFirstSelectedGrunt = myUnits[i];
				}

				if (myFirstSelectedRanger == nullptr && myUnits[i]->GetUnitType() == eUnitType::RANGER)
				{
					myRangerActionButtons->At(0)->SetValue(myUnits[i]->GetComponent<ActorComponent>()->GetRangerCooldown(), myUnits[i]->GetComponent<ActorComponent>()->GetRangerMaxCooldown());
					myFirstSelectedRanger = myUnits[i];
				}

				if (myFirstSelectedTank == nullptr && myUnits[i]->GetUnitType() == eUnitType::TANK)
				{
					myTankActionButtons->At(0)->SetValue(myUnits[i]->GetComponent<EnrageComponent>()->GetCooldown(), myUnits[i]->GetComponent<EnrageComponent>()->GetMaxCooldown());
					myFirstSelectedTank = myUnits[i];
				}
			}
		}
	}

	Widget* UnitActionWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (myIsUnitSelected == true)
		{
			if (mySelectedType == eEntityType::UNIT)
			{
				Widget* widget = nullptr;

				if (myFirstSelectedGrunt != nullptr)
				{
					Widget* gruntWidget = myFirstSelectedGrunt->GetComponent<PromotionComponent>()->GetPromoted() ? 
						myGruntActionButtons->MouseIsOver(aPosition - myPosition, 0) : myGruntActionButtons->MouseIsOver(aPosition - myPosition, 1);
					widget = gruntWidget == nullptr ? widget : gruntWidget;
				}
				if (myFirstSelectedRanger != nullptr)
				{
					Widget* rangerWidget = myFirstSelectedRanger->GetComponent<PromotionComponent>()->GetPromoted() ?
						myRangerActionButtons->MouseIsOver(aPosition - myPosition, 0) : myRangerActionButtons->MouseIsOver(aPosition - myPosition, 1);
					widget = rangerWidget == nullptr ? widget : rangerWidget;
				}
				if (myFirstSelectedTank != nullptr)
				{
					Widget* tankWidget = myFirstSelectedTank->GetComponent<PromotionComponent>()->GetPromoted() ?
						myTankActionButtons->MouseIsOver(aPosition - myPosition, 0) : myTankActionButtons->MouseIsOver(aPosition - myPosition, 1);
					widget = tankWidget == nullptr ? widget : tankWidget;
				}

				if (widget != nullptr)
				{
					return widget;
				}

				return myUnitActionButtons->MouseIsOver(aPosition - myPosition);
			}
			else if (mySelectedType == eEntityType::BASE_BUILING)
			{
				Widget* widget = myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 0);
				widget = widget != nullptr ? widget : myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 5);

				if (myHasUnlockedRanger == true && widget == nullptr)
				{
					widget = myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 1);
					widget = widget != nullptr ? widget : myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 6);
				}
				else if (myHasUnlockedRanger == false && widget == nullptr)
				{
					widget = myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 2);
					widget = widget != nullptr ? widget : myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 8);
				}
				
				if (myHasUnlockedTank == true && widget == nullptr)
				{
					widget = myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 3);
					widget = widget != nullptr ? widget : myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 7);
				}
				else if (myHasUnlockedTank == false && widget == nullptr)
				{
					widget = myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 4);
					widget = widget != nullptr ? widget : myBuildingActionButtons->MouseIsOver(aPosition - myPosition, 9);
				}

				return widget;
			}
		}
		return nullptr;
	}

	void UnitActionWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myUnitActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myBuildingActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myGruntActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myRangerActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
		myTankActionButtons->OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
	}

	WidgetContainer* UnitActionWidget::ReadContainer(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, CU::Vector2<float> aSize)
	{
		GUI::WidgetContainer* container = new WidgetContainer(nullptr, aSize, false);
		tinyxml2::XMLElement* widgetElement = aReader->FindFirstChild(anXMLElement, "widget");
		for (; widgetElement != nullptr; widgetElement = aReader->FindNextElement(widgetElement))
		{
			std::string type = "";

			aReader->ForceReadAttribute(widgetElement, "type", type);

			if (type == "button")
			{
				ButtonWidget* button = new ButtonWidget(aReader, widgetElement, myPlayer);
				container->AddWidget(button);
			}
			else if (type == "upgrade_button")
			{
				UpgradeButtonWidget* upgradeButtonWidget = new UpgradeButtonWidget(aReader, widgetElement, myPlayer);
				container->AddWidget(upgradeButtonWidget);
			}
			else if (type == "ability_button")
			{
				AbilityButton* upgradeButtonWidget = new AbilityButton(aReader, widgetElement, myPlayer);
				container->AddWidget(upgradeButtonWidget);
			}
		}
		return container;
	}
}