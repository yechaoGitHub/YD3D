#include "ReadbackHelper.h"

namespace YD3D 
{
	GraphicReadbackBuffer ReadbackHelper::_READBACKER_;

	ReadbackHelper::ReadbackHelper()
	{
	}

	ReadbackHelper::~ReadbackHelper()
	{
	}

	void ReadbackHelper::Initialize(ID3D12Device* device)
	{
		_READBACKER_.Create(device, 1024);
	}

	void ReadbackHelper::ReadbackBuffer(GraphicResource* destRes, uint64_t destOffset, uint64_t dataLength)
	{
		ReadbackLayout layout;
		layout.DestResource = destRes->Resource();
		layout.DataLength = dataLength;
		layout.BufferStartOffset = destOffset;
	}

	void ReadbackHelper::ReadbackTexture(GraphicResource* destRes, uint32_t startSubresourceIndex, uint32_t subresourceCount)
	{
	}

	void ReadbackHelper::Excute()
	{

	}

	const uint8_t* ReadbackHelper::GetReadbackData(GraphicResource* res, uint64_t* length)
	{
		return nullptr;
	}
}
