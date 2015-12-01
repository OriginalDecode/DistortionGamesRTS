#include "stdafx.h"
#include <Engine.h>
#include <Sprite.h>
#include "WidgetContainer.h"
#include <XMLReader.h>

namespace GUI
{
	WidgetContainer::WidgetContainer(Prism::Sprite* aBackgroundSprite, const CU::Vector2<float>& aSize)
		: myBackground(aBackgroundSprite)
		, myWidgets(8)
	{
		mySize = aSize;
	}

	WidgetContainer::~WidgetContainer()
	{
		myWidgets.DeleteAll();
		SAFE_DELETE(myBackground);
	}

	void WidgetContainer::AddWidget(Widget* aWidget)
	{
		myWidgets.Add(aWidget);
	}

	void WidgetContainer::Update()
	{
		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->Update();
		}
	}

	void WidgetContainer::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myBackground != nullptr)
		{
			myBackground->Render(myPosition);
		}

		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->Render(myPosition + aParentPosition);
		}
	}

	Widget* WidgetContainer::MouseIsOver(const CU::Vector2<float>& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			for (int i = 0; i < myWidgets.Size(); i++)
			{
				if (myWidgets[i]->IsVisible() == true && myWidgets[i]->IsInside(aPosition - myPosition) == true)
				{
					Widget* childWidget = myWidgets[i]->MouseIsOver(aPosition - myPosition);
					if (childWidget != nullptr)
					{
						return childWidget;
					}
				}
			}

			return this;
		}
		return nullptr;
	}

	void WidgetContainer::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		for (int i = 0; i < myWidgets.Size(); i++)
		{
			myWidgets[i]->OnResize(aNewSize, anOldSize);
		}
		if (myBackground != nullptr)
		{
			CU::Vector2<float> ratio = myBackground->GetSize() / anOldSize;
			myBackground->SetSize(aNewSize * ratio, { 0.f, 0.f });
		}
	}
}