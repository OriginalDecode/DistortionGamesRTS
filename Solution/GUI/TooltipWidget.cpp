#include "stdafx.h"
#include <Engine.h>
#include "TooltipWidget.h"
#include "GUIManager.h"

struct TooltipInfo
{
	std::string myName;
	std::string myText;
	int myGoldCost;
	int myArftifactCost;
	int mySupplyCost;
	float myCooldown;
};


namespace GUI
{
	TooltipWidget::TooltipWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const GUIManager* aGuiManager)
		: Widget()
		, myGuiManager(aGuiManager)
		, myBackground(nullptr)
	{
		std::string backgroundPath;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "x", myTextPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "textposition"), "y", myTextPosition.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", backgroundPath);

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite(backgroundPath, mySize);
	}

	TooltipWidget::~TooltipWidget()
	{
		SAFE_DELETE(myBackground);
	}

	void TooltipWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myActiveWidget = myGuiManager->GetActiveWidget();

		if (myActiveWidget != nullptr && myActiveWidget->GetHoverText() != "")
		{
			myBackground->Render(myPosition + aParentPosition);
			Prism::Engine::GetInstance()->PrintText(myActiveWidget->GetHoverText(), myTextPosition + myPosition + aParentPosition, Prism::eTextType::RELEASE_TEXT);
		}
	}

	Widget* TooltipWidget::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		return nullptr;
	}

	void TooltipWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize, bool aIsFullScreen)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize, aIsFullScreen);
	}
}
