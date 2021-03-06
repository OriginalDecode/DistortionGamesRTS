#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityFactory.h"
#include "EntityEnumConverter.h"
#include <string>


EntityFactory* EntityFactory::myInstance = nullptr;

EntityFactory* EntityFactory::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EntityFactory();
	}
	return myInstance;
}

void EntityFactory::Destroy()
{
	SAFE_DELETE(myInstance);
}

void EntityFactory::LoadEntities(const char* aEntityListXML)
{
	XMLReader entityListDocument;
	entityListDocument.OpenDocument(aEntityListXML);

	tinyxml2::XMLElement* rootElement = entityListDocument.ForceFindFirstChild("root");

	for (tinyxml2::XMLElement* e = entityListDocument.FindFirstChild(rootElement); e != nullptr;
		e = entityListDocument.FindNextElement(e))
	{
		std::string entityPath;
		entityListDocument.ForceReadAttribute(e, "src", entityPath);
		LoadEntity(entityPath.c_str());
	}

	entityListDocument.CloseDocument();
}

Entity* EntityFactory::CreateEntity(eOwnerType aOwner, eEntityType aType, Prism::eOctreeType aOctreeType,
	Prism::Scene& aScene, CU::Vector3f aPostion, const Prism::Terrain& aTerrian, const CU::Vector3f& aRotation,
	const CU::Vector3f& aScale)
{
	if (myInstance->myLoadedEntityData.find(aType) != myInstance->myLoadedEntityData.end())
	{
		EntityData loadedEntityData = myInstance->myLoadedEntityData.find(aType)->second;
		Entity* newEntity = new Entity(aOwner, aOctreeType, loadedEntityData, aScene, aPostion, aTerrian, aRotation
			, aScale, eUnitType::NOT_A_UNIT);
		return newEntity;
	}
	DL_ASSERT("Entity not found.");
	return nullptr;
}

Entity* EntityFactory::CreateEntity(eOwnerType aOwner, eEntityType aType, std::string aSubType, Prism::eOctreeType aOctreeType,
	Prism::Scene& aScene, CU::Vector3f aPostion, const Prism::Terrain& aTerrian,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (aType == eEntityType::PROP || aType == eEntityType::UNIT || aType == eEntityType::ARTIFACT)
	{
		if (myInstance->myLoadedSubEntityData.find(aSubType) != myInstance->myLoadedSubEntityData.end())
		{
			EntityData loadedEntityData = myInstance->myLoadedSubEntityData.find(aSubType)->second;
			Entity* newEntity = new Entity(aOwner, aOctreeType, loadedEntityData, aScene, aPostion, aTerrian, aRotation
				, aScale, loadedEntityData.myUnitType);
			newEntity->mySubType = aSubType;
			return newEntity;
		}
	}
	std::string errorMessage = "SubType " + aSubType + " not found.";
	DL_ASSERT(errorMessage);
	return nullptr;
}

void EntityFactory::LoadEntity(const char* aEntityPath)
{
	XMLReader entityDocument;
	entityDocument.OpenDocument(aEntityPath);

	tinyxml2::XMLElement* rootElement = entityDocument.ForceFindFirstChild("root");
	tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity");

	EntityData newData;
	std::string entityType;
	std::string entitySubType;
	entityDocument.ForceReadAttribute(entityElement, "type", entityType);
	newData.myType = EntityEnumConverter::ConvertStringToEntityType(CU::ToLower(entityType));
	if (newData.myType == eEntityType::PROP || newData.myType == eEntityType::UNIT || newData.myType == eEntityType::ARTIFACT)
	{
		entityDocument.ForceReadAttribute(entityElement, "subType", entitySubType);
		newData.mySubType = CU::ToLower(entitySubType);
		newData.myUnitType = eUnitType::NOT_A_UNIT;
		if (newData.myType == eEntityType::UNIT)
		{
			newData.myUnitType = EntityEnumConverter::ConvertStringToUnitType(newData.mySubType);
		}
	}


	for (tinyxml2::XMLElement* e = entityDocument.FindFirstChild(entityElement); e != nullptr;
		e = entityDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("AnimationComponent"))
		{
			if (newData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You have a GraphicsComponent so you can't have a AnimationComponent");
			if (newData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You already have a AnimationComponent");

			myComponentLoader->LoadAnimationComponent(entityDocument, e, newData.myAnimationData);
		}
		else if (elementName == CU::ToLower("GraphicsComponent"))
		{
			if (newData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You have a AnimationComponent so you can't have a GraphicsComponent");
			if (newData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You already have a GraphicsComponent");

			myComponentLoader->LoadGraphicsComponent(entityDocument, e, newData.myGraphicsData);
		}
		else if (elementName == CU::ToLower("BuildingComponent"))
		{
			if (newData.myBuildingData.myExistsInEntity == true) DL_ASSERT("You already have a BuildingComponent");

			myComponentLoader->LoadBuidlingComponent(entityDocument, e, newData.myBuildingData);
		}
		else if (elementName == CU::ToLower("TriggerComponent"))
		{
			if (newData.myTriggerData.myExistsInEntity == true) DL_ASSERT("You already have a TriggerComponent");

			myComponentLoader->LoadTriggerComponent(entityDocument, e, newData.myTriggerData);
		}
		else if (elementName == CU::ToLower("CollisionComponent"))
		{
			if (newData.myCollisionData.myExistsInEntity == true) DL_ASSERT("You already have a CollisionComponent");

			myComponentLoader->LoadCollisionComponent(entityDocument, e, newData.myCollisionData);
		}
		else if (elementName == CU::ToLower("ActorComponent"))
		{
			if (newData.myActorData.myExistsInEntity == true) DL_ASSERT("You already have a ActorComponent");

			myComponentLoader->LoadActorComponent(entityDocument, e, newData.myActorData);
		}
		else if (elementName == CU::ToLower("ControllerComponent"))
		{
			if (newData.myControllerData.myExistsInEntity == true) DL_ASSERT("You already have a ControllerComponent");

			myComponentLoader->LoadControllerComponent(entityDocument, e, newData.myControllerData);
		}
		else if (elementName == CU::ToLower("HealthComponent"))
		{
			if (newData.myHealthData.myExistsInEntity == true) DL_ASSERT("You already have a HealthComponent");

			myComponentLoader->LoadHealthComponent(entityDocument, e, newData.myHealthData);
		}
		else if (elementName == CU::ToLower("EnrageComponent"))
		{
			if (newData.myEnrageData.myExistsInEntity == true) DL_ASSERT("You already have a EnrageComponent");

			myComponentLoader->LoadEnrageComponent(entityDocument, e, newData.myEnrageData);
		}
		else if (elementName == CU::ToLower("GrenadeComponent"))
		{
			if (newData.myGrenadeData.myExistsInEntity == true) DL_ASSERT("You already have a GrenadeComponent");

			myComponentLoader->LoadGrenadeComponent(entityDocument, e, newData.myGrenadeData);
		}
		else if (elementName == CU::ToLower("PromotionComponent"))
		{
			if (newData.myPromotionData.myExistsInEntity == true) DL_ASSERT("You already have a PromotionComponent");

			myComponentLoader->LoadPromotionComponent(entityDocument, e, newData.myPromotionData);
		}
		else if (elementName == CU::ToLower("SelectionComponent"))
		{
			if (newData.mySelectionData.myExistsInEntity == true) DL_ASSERT("You already have a SelectionComponent");

			myComponentLoader->LoadSelectionComponent(entityDocument, e, newData.mySelectionData);
		}
		else if (elementName == CU::ToLower("SoundComponent"))
		{
			if (newData.mySoundData.myExistsInEntity == true) DL_ASSERT("You already have a SoundComponent");

			myComponentLoader->LoadSoundComponent(entityDocument, e, newData.mySoundData);
		}
		else
		{
			std::string errorMessage = "The component " + elementName +
				" is not Supported. Please check if you wrote right else tell a programmer.";
			DL_ASSERT(errorMessage.c_str());
		}
	}
	if (newData.myType == eEntityType::PROP || newData.myType == eEntityType::UNIT || newData.myType == eEntityType::ARTIFACT)
	{
		myLoadedSubEntityData[newData.mySubType] = newData;
	}
	else
	{
		myLoadedEntityData[newData.myType] = newData;
	}

	entityDocument.CloseDocument();
}

EntityFactory::EntityFactory()
{
	myComponentLoader = new ComponentLoader();
}


EntityFactory::~EntityFactory()
{
	SAFE_DELETE(myComponentLoader);
}