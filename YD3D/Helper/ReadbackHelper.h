#include "YD3D_Header.h"
#include "Resource/GraphicReadbackBuffer.h"

namespace YD3D
{
	class ReadbackHelper 
	{
		struct TextureLayout
		{
			uint32_t StartSubresourceIndex;
			uint32_t SubresourceCount;
		};

		struct ReadbackLayout
		{
			ReadbackLayout() :
				Offset(0),
				DataLength(0),
				IsBuffer(false),
				DestResource(nullptr)
			{

			}

			uint64_t		Offset;
			uint64_t		DataLength;
			bool			IsBuffer;
			ID3D12Resource* DestResource;

			union
			{
				uint64_t BufferStartOffset;
				TextureLayout TextureLayout;
			};
		};

	public:
		ReadbackHelper();
		~ReadbackHelper();

		static void Initialize(ID3D12Device* device);
		
		void ReadbackBuffer(GraphicResource* destRes, uint64_t destOffset, uint64_t dataLength);
		void ReadbackTexture(GraphicResource* destRes, uint32_t startSubresourceIndex, uint32_t subresourceCount);
		void Excute();
		const uint8_t* GetReadbackData(GraphicResource *res, uint64_t *length);

	private:
		static GraphicReadbackBuffer							_READBACKER_;
		uint64_t												mReadbackDataLength;
		std::vector<ReadbackLayout>								mVecLayout;
		std::unordered_map<ID3D12Resource*, uint32_t>			mMapLayoutIndex;
	};

};
