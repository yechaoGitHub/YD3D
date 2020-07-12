#include "ResourcePackage.h"

namespace YD3D 
{
	ResourcePackage::ResourcePackage():
		State(EResourcePackageState::EINIT),
		mHasCallback(false)
	{

	}

	ResourcePackage::~ResourcePackage()
	{

	}

	void ResourcePackage::SetCallback(GraphicTaskCallbackFunction&& callback)
	{
		mHasCallback = true;
		mCallback = std::move(callback);
	}

	bool ResourcePackage::HasCallback()
	{
		return mHasCallback;
	}

	GraphicTaskCallbackFunction&& ResourcePackage::MoveCallback()
	{
		return std::move(mCallback);
	}

	void ResourcePackage::ResourcePackageCallBack(D3D12_COMMAND_LIST_TYPE type, uint64_t fence)
	{
		State.set_state(EResourcePackageState::ERENDERED);

		if (mHasCallback)
		{
			mCallback(type, fence);
		}
	}

}

