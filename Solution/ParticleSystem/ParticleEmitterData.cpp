#include "stdafx.h"

#include <d3dx11effect.h>
#include <Effect.h>
#include <EffectContainer.h>
#include <Engine.h>
#include "ParticleEmitterData.h"
#include <TextureContainer.h>
#include <XMLReader.h>

namespace Prism
{
	ParticleEmitterData::ParticleEmitterData()
		: myTexture(nullptr)
		, myEffect(nullptr)
		, myFileName("not initialized")
		, myInputLayout(nullptr)
		, myTechniqueDesc(nullptr)
		, myMinRotation(0.f)
		, myMaxRotation(0.f)
		, myRotationDelta(0.f)
		, mySpeedMultiplier(0.f)
		, myEffectName("not initialized")
		, myTextureName("not initialized")
		, myParticlesLifeTime(0.f)
		, myEmitterLifeTime(0.f)
		, myEmissionLifeTime(0.f)
		, myEmissionRate(0.f)
		, myEmissionRateDelta(0.f)
		, myMinScale(0.f)
		, myMaxScale(0.f)
		, myParticlesPerEmitt(0)
		, myMaxParticleAmount(0)
		, myIsActiveAtStart(false)
		, myUseEmitterLifeTime(false)
		, myUseAlphaDelta(false)
		, myIsHollow(false)
		, myIsCircle(false)
	{
	}

	ParticleEmitterData::~ParticleEmitterData()
	{
		if (myInputLayout != nullptr)
		{
			SAFE_RELEASE(myInputLayout);
		}
		SAFE_DELETE(myTechniqueDesc);
	
	}

	void ParticleEmitterData::LoadDataFile(const std::string& aFilePath)
	{
		XMLReader read;
		myFileName = aFilePath;

		read.OpenDocument(aFilePath);
		tinyxml2::XMLElement* emitter = read.ForceFindFirstChild("Emitter");
		tinyxml2::XMLElement* element;
		element = read.ForceFindFirstChild(emitter, "Shader");
		read.ReadAttribute(element, "filepath", myEffectName);

		element = read.ForceFindFirstChild(emitter, "Texture");
		read.ReadAttribute(element, "filepath", myTextureName);

		element = read.ForceFindFirstChild(emitter, "CircleEmitter");
		read.ReadAttribute(element, "value", myIsCircle);

		element = read.ForceFindFirstChild(emitter, "HollowEmitter");
		read.ReadAttribute(element, "value", myIsHollow);

		element = read.ForceFindFirstChild(emitter, "EmitterSize");
		read.ReadAttribute(element, "x", "y", "z", myEmitterSize);
		myEmitterSize *= 0.5f;

		element = read.ForceFindFirstChild(emitter, "ParticleRotation");
		read.ReadAttribute(element, "min", myMinRotation);
		read.ReadAttribute(element, "max", myMaxRotation);

		element = read.ForceFindFirstChild(emitter, "ParticleRotationDelta");
		read.ReadAttribute(element, "value", myRotationDelta);

		element = read.ForceFindFirstChild(emitter, "ParticleMaxVelocity");
		read.ReadAttribute(element, "x", "y", "z", myMaxVelocity);

		element = read.ForceFindFirstChild(emitter, "ParticleMinVelocity");
		read.ReadAttribute(element, "x", "y", "z", myMinVelocity);

		element = read.ForceFindFirstChild(emitter, "ParticleSpeedMultiplier");
		read.ReadAttribute(element, "value", mySpeedMultiplier);

		element = read.ForceFindFirstChild(emitter, "ParticleStartColor");
		read.ReadAttribute(element, "r", "g", "b", myData.myStartColor);

		element = read.ForceFindFirstChild(emitter, "ParticleEndColor");
		read.ReadAttribute(element, "r", "g", "b", myData.myEndColor);

		element = read.ForceFindFirstChild(emitter, "ParticleLifeTime");
		read.ReadAttribute(element, "value", myParticlesLifeTime);

		element = read.ForceFindFirstChild(emitter, "EmitterLifeTime");
		read.ReadAttribute(element, "value", myEmitterLifeTime);
		if (myEmitterLifeTime < 0)
		{
			myUseEmitterLifeTime = false;
		}
		else
		{
			myUseEmitterLifeTime = true;
		}
		element = read.ForceFindFirstChild(emitter, "EmittsPerSecond");
		read.ReadAttribute(element, "value", myEmissionRate);
		myEmissionRate = 1 / myEmissionRate;

		element = read.ForceFindFirstChild(emitter, "ParticlesPerEmitt");
		read.ReadAttribute(element, "value", myParticlesPerEmitt);

		//Dead code, use in the future?
		//element = read.ForceFindFirstChild(emitter, "EmissionRateDelta");
		//read.ReadAttribute(element, "value", myEmissionRateDelta);


		element = read.ForceFindFirstChild(emitter, "ParticleScale");
		read.ReadAttribute(element, "min", myMinScale);
		read.ReadAttribute(element, "max", myMaxScale);


		element = read.ForceFindFirstChild(emitter, "ParticleAlphaDelta");
		read.ReadAttribute(element, "value", myData.myAlphaDelta);

		element = read.ForceFindFirstChild(emitter, "ParticleSizeDelta");
		read.ReadAttribute(element, "value", myData.mySizeDelta);

		element = read.ForceFindFirstChild(emitter, "ParticleAlphaStart");
		read.ReadAttribute(element, "value", myData.myStartAlpha);

		element = read.ForceFindFirstChild(emitter, "ActiveAtStart");
		read.ReadAttribute(element, "value", myIsActiveAtStart);

		element = read.ForceFindFirstChild(emitter, "UseAlphaDelta");
		read.ReadAttribute(element, "value", myUseAlphaDelta);

		read.CloseDocument();

		myData.myStartColor /= 255.f;
		myData.myEndColor /= 255.f;

		myData.myMaxStartSize = myMaxScale;
		myData.myMinStartSize = myMinScale;
		
		myData.myMaxVelocity = myMaxVelocity;;
		myData.myMinVelocity = myMinVelocity;;

		myData.myLifeTime = myParticlesLifeTime;

		myTexture = TextureContainer::GetInstance()->GetTexture(myTextureName.c_str());
		myEffect = EffectContainer::GetInstance()->GetEffect(myEffectName.c_str());
		
		CreateInputLayout();
		myTechniqueDesc = new _D3DX11_TECHNIQUE_DESC();

	}

	void ParticleEmitterData::CreateInputLayout()
	{
		HRESULT hr;

		D3DX11_PASS_DESC passDesc;
		hr = myEffect->GetTechnique(false)->GetPassByIndex(0)->GetDesc(&passDesc);
		DL_ASSERT_EXP(!FAILED(hr), "[ParticleEmitterData](CreateInputLayout) : Failed to get Pass Description!");

		const D3D11_INPUT_ELEMENT_DESC VertexParticleLayout[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ALPHA",		0, DXGI_FORMAT_R32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE",		0, DXGI_FORMAT_R32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TIME",		0, DXGI_FORMAT_R32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ROTATION",	0, DXGI_FORMAT_R32_FLOAT,		0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		UINT size = ARRAYSIZE(VertexParticleLayout);

		hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(VertexParticleLayout
			, size
			, passDesc.pIAInputSignature
			, passDesc.IAInputSignatureSize
			, &myInputLayout);
		DL_ASSERT_EXP(!FAILED(hr), "[ParticleEmitterData](CreateInputLayout) : Failed to Create InputLayout!");

		Engine::GetInstance()->SetDebugName(myInputLayout, "ParticleEmitterData::myInputLayout");
	}
}