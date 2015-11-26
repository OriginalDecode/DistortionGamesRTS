#include "stdafx.h"
#include "Cube3DRenderer.h"
#include "DebugDrawer.h"
#include "Engine.h"
#include "Line3DRenderer.h"


#define WHITE_DEBUG CU::Vector4<float>(1.f, 1.f, 1.f, 1.f)
#define BLACK_DEBUG CU::Vector4<float>(0.f, 0.f, 0.f, 1.f)
#define RED_DEBUG CU::Vector4<float>(1.f, 0.f, 0.f, 1.f)
#define GREEN_DEBUG CU::Vector4<float>(0.f, 1.f, 0.f, 1.f)
#define BLUE_DEBUG CU::Vector4<float>(0.f, 0.f, 1.f, 1.f)
#define PINK_DEBUG CU::Vector4<float>(1.f, 0.f, 1.f, 1.f)
#define YELLOW_DEBUG CU::Vector4<float>(1.f, 1.f, 0.f, 1.f)
#define NOT_USED_DEBUG CU::Vector4<float>(-1.f, -1.f, -1.f, -1.f)


namespace Prism
{
	DebugDrawer* DebugDrawer::myInstance = nullptr;
	DebugDrawer* DebugDrawer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new DebugDrawer();
		}

		return myInstance;
	}

	void DebugDrawer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	//void DebugDrawer::RenderLine2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderLine3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float> aSecondPoint
		, eColorDebug aColor, eColorDebug aSecondColor)
	{
		my3DLines.Add(Line3D(aFirstPoint, aSecondPoint, GetColor(aColor), GetColor(aSecondColor)));
	}

	//void DebugDrawer::RenderArrow2D(const CU::Vector2<float>& aFirstPoint, const CU::Vector2<float> aSecondPoint
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderArrow3D(const CU::Vector3<float>& aFirstPoint, const CU::Vector3<float>& aSecondPoint
		, eColorDebug aColor)
	{
		DL_ASSERT("Not Implemented.");
	}

	void DebugDrawer::RenderBox(const CU::Vector3<float>& aPosition, float aSize, eColorDebug aColor
		, bool aWireFrame)
	{
		myCube3DRenderer->AddCube(aPosition, aSize, GetColor(aColor), aWireFrame);
	}

	void DebugDrawer::RenderSphere(const CU::Vector3<float>& aPosition, float aSize, eColorDebug aColor
		, bool aWireFrame)
	{
		DL_ASSERT("Not Implemented.");
	}

	//void DebugDrawer::RenderText2D(const std::string& aText, const CU::Vector2<float>& aPosition
	//	, const CU::Vector4<float>& aColor)
	//{
	//	DL_ASSERT("Not Implemented.");
	//}

	void DebugDrawer::RenderText3D(const std::string& aText, const CU::Vector3<float>& aPosition
		, eColorDebug aColor)
	{
		DL_ASSERT("Not Implemented.");
	}

	void DebugDrawer::Render(const Camera& aCamera)
	{
		myLine3DRenderer->Render(my3DLines, aCamera);
		myCube3DRenderer->Render(aCamera);

		my3DLines.RemoveAll();
	}

	DebugDrawer::DebugDrawer()
		: my3DLines(1024)
		, myLine3DRenderer(new Line3DRenderer())
		, myCube3DRenderer(new Cube3DRenderer())
	{

	}

	DebugDrawer::~DebugDrawer()
	{
		SAFE_DELETE(myLine3DRenderer);
		SAFE_DELETE(myCube3DRenderer);
	}

	CU::Vector4<float> DebugDrawer::GetColor(eColorDebug aColor) const
	{
		CU::Vector4<float> color;

		switch (aColor)
		{
			case eColorDebug::WHITE:
				color = WHITE_DEBUG;
				break;
			case eColorDebug::BLACK:
				color = BLACK_DEBUG;
				break;
			case eColorDebug::RED:
				color = RED_DEBUG;
				break;
			case eColorDebug::GREEN:
				color = GREEN_DEBUG;
				break;
			case eColorDebug::BLUE:
				color = BLUE_DEBUG;
				break;
			case eColorDebug::PINK:
				color = PINK_DEBUG;
				break;
			case eColorDebug::YELLOW:
				color = YELLOW_DEBUG;
				break;
			case eColorDebug::NOT_USED:
				color = NOT_USED_DEBUG;
				break;
			default:
				DL_ASSERT("Unknown color in DEBUGDRAWER");
				color = WHITE_DEBUG;
				break;
		}
		return color;
	}
}