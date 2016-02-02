#pragma once
#include "Defines.h"
#include <GrowingArray.h>
#include "LightStructs.h"
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Instance;
	class PointLight;
	class SpotLight;
	class SpotLightShadow;
	class Terrain;
	class InstancingHelper;
#ifdef SCENE_USE_OCTREE
	class Octree;
#endif

	class Scene
	{
	public:
		Scene(const Camera& aCamera, Terrain& aTerrain);
		~Scene();

		void Render(bool aRenderNavMeshLines);
		void Render(bool aRenderNavMeshLines, Texture* aFogOfWarTexture, SpotLightShadow* aShadowSpotLight);

		void AddInstance(Instance* aInstance);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);

		void RemoveInstance(Instance* aInstance);

		void SetCamera(const Camera& aCamera);
		const Camera* GetCamera() const;
		
	private:
		void operator=(Scene&) = delete;
#ifdef SCENE_USE_OCTREE
		Octree* myOctree;
#endif
		CU::GrowingArray<Instance*> myInstances;
		CU::GrowingArray<Instance*> myDynamicInstances;
		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;
		InstancingHelper* myInstancingHelper;

		const Camera* myCamera;
		Terrain& myTerrain;

		CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS> myDirectionalLightData;
		CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS> myPointLightData;
		CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS> mySpotLightData;
	};



	inline const Camera* Scene::GetCamera() const
	{
		return myCamera;
	}
}