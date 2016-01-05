#pragma once
#include <Subscriber.h>
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
	class Text;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class TextWidget : public Widget, public Subscriber
	{
	public:
		TextWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~TextWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

		void SetPosition(const CU::Vector2<float>& aPosition) override;

		void ReceiveMessage(const TextMessage& aMessage) override;

	private:

		Prism::SpriteProxy* myBackground;
		Prism::Text* myText;

	};
}