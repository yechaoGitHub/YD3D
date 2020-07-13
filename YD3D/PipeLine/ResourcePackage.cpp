#include "ResourcePackage.h"

namespace YD3D 
{
	ResourcePackage::ResourcePackage():
		State(EResourcePackageState::EINIT),
		mHasUserCallback(false)
	{
		mPackageCallback = std::bind(&ResourcePackage::ResourcePackageCallBack, this, std::placeholders::_1, std::placeholders::_2);
	}

	ResourcePackage::~ResourcePackage()
	{

	}

	void ResourcePackage::BindCallback(GraphicTaskCallbackFunction&& UserCallback)
	{
		mHasUserCallback = true;
		mUserCallback = std::move(UserCallback);
	}

	GraphicTaskCallbackFunction& ResourcePackage::UserCallback()
	{
		return mUserCallback;
	}

	GraphicTaskCallbackFunction& ResourcePackage::PackageCallback()
	{
		return mPackageCallback;
	}

	void ResourcePackage::ResourcePackageCallBack(D3D12_COMMAND_LIST_TYPE type, uint64_t fence)
	{
		State.set_state(EResourcePackageState::ERENDERED);

		if (mHasUserCallback)
		{
			mUserCallback(type, fence);
		}
	}

}

