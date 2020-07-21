#pragma once
#include "YD3D_Header.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Resource/GraphicVertexBuffer.h"
#include "Resource/GraphicIndexBuffer.h"
#include "Resource/GraphicUploadBuffer.h"
#include "Resource/GraphicConstBuffer.hpp"

namespace YD3D
{
	struct DrawParam 
	{
		DrawParam() :
			Model(nullptr),
			IndexCountPerInstance(0),
			StartIndexLocation(0),
			BaseVertexLocation(0),
			StartInstanceLocation(0)
		{
		}

		Model*		Model;
		uint32_t	IndexCountPerInstance;
		uint32_t	StartIndexLocation;	
		int32_t		BaseVertexLocation;
		uint32_t	StartInstanceLocation;
	};

	typedef std::vector<gc_ptr<Model>, gc_allocator<gc_ptr<Model>>> VecModel;
	typedef std::unordered_map<Model*, DrawParam>					MapDrawParam;

	ALIGN_16 struct SceneInfo
	{
		DirectX::XMFLOAT4X4			View = Identity4x4();
		DirectX::XMFLOAT4X4			Project = Identity4x4();
		DirectX::XMFLOAT4X4			ViewProject = Identity4x4();
		ALIGN_16 DirectX::XMFLOAT3	CameraPos = {};
		ALIGN_16 DirectX::XMFLOAT3	CameraDir = {};
				 uint32_t			PointLightCount = 0;
				 uint32_t			SpotLightCount = 0;
		ALIGN_16 DirectX::XMFLOAT3	AmbientLight = {};
		ALIGN_16 DirectX::XMFLOAT3  DirectionalLightDir = {};
		ALIGN_16 DirectX::XMFLOAT3  DirectionalLightStrength = {};
	};

	enum ESceneState { FREE = 0, SCENE_UPDATING = 0x01, RENDER_UPLOADING = 0x02, CAMERA_DIRTY = 0x04, LIGHT_PARAM_DIRTY = 0x10, LIGHT_DIRTY = 0x20, DRAW_PARAM_DIRTY = 0x40, VERTEX_INDEX_DIRTY = 0x80 };

	class Scene : public enable_gc_ptr_form_raw
	{
		typedef std::function<bool(ystate<ESceneState>&)> StateCheckFunction;
		typedef std::function<void(ystate<ESceneState>&)> StateSetFunction;

	public:
		enum { need_clear_up_gc_ptr = 0 };

		Scene();
		~Scene();

		bool Create(ID3D12Device *device);
		bool AddModel(const Model *model);
		void UpdateDrawParam();
		
		void UpdateGraphicResource(bool wait = false);

		const D3D12_INDEX_BUFFER_VIEW& IndexView();
		const D3D12_VERTEX_BUFFER_VIEW& VertexView();
		const MapDrawParam& GetDrawParam();
		
		void Walk(float value);
		void Strafe(float value);
		void Pitch(float angle);
		void RotateY(float angle);
		void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);
		void SetLens(float fovY, float aspect, float zn, float zf);

		void AddPointLight(const DirectX::XMFLOAT3& color, const float &intensity, const DirectX::XMFLOAT3 &position, const float &radius);
		void AddSpotLight(const float& radius, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &direction);
		void UpdateLightParam();

		ystate<ESceneState>& State();
		bool StateBarrier(StateCheckFunction stateCheckFunc, StateSetFunction stateSetFunc);
		
		GraphicConstBuffer<SceneInfo>*			GraphicSceneInfo();
		GraphicConstBuffer<LightDataStruct>*	GraphicLightInfo();
		
	private:
		ID3D12Device*									mDevice;
		uint64_t										mVertexBufferLength;
		uint64_t										mIndexBufferLength;
		GraphicVertexBuffer								mVertexBuffer;
		GraphicIndexBuffer								mIndexBuffer;
		GraphicUploadBuffer								mUploadBuffer;
		VecModel										mModels;
		MapDrawParam									mDrawParam;

		Camera											mCamera;
		SceneInfo										mSceneInfo;
		std::vector<LightDataStruct>					mPointLights;
		std::vector<LightDataStruct>					mSpotLights;
		gc_ptr<GraphicConstBuffer<SceneInfo>>			mGpuSceneInfo;
		gc_ptr<GraphicConstBuffer<LightDataStruct>>		mGpuLightInfo;
		ystate<ESceneState>								mState;

		bool GraphicTaskUploadVertexIndexData(ID3D12GraphicsCommandList *commandList);
	};
};

