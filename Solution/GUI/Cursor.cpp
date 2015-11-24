#include "stdafx.h"
#include "Cursor.h"
#include "../InputWrapper/InputWrapper.h"
#include <Sprite.h>

namespace GUI
{
	Cursor::Cursor(CU::InputWrapper* anInputWrapper, const CU::Vector2<int>& aWindowSize)
		: myInputWrapper(anInputWrapper)
		, mySprite(nullptr)
	{
		myWindowSize.x = float(aWindowSize.x);
		myWindowSize.y = float(aWindowSize.y);

		mySprite = new Prism::Sprite("Data/Resource/Texture/Star.dds", { 100.f, 100.f }, { 50.f, 50.f });
		myPosition = myWindowSize / 2.f;
	}

	Cursor::~Cursor()
	{
		delete mySprite;
		mySprite = nullptr;
	}

	void Cursor::Update()
	{
		myPosition.x += myInputWrapper->GetMouseDX();
		myPosition.y += -myInputWrapper->GetMouseDY();
	}

	void Cursor::Render()
	{
		mySprite->Render(myPosition);
	}

	const CU::Vector2<float>& Cursor::GetMousePosition() const
	{
		return myPosition;
	}
}