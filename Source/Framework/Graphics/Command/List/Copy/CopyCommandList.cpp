#include "CopyCommandList.h"

FWK::Graphics::CopyCommandList::CopyCommandList() : 
	CommandListBase(Constant::k_createCommandListTypeCopy)
{}
FWK::Graphics::CopyCommandList::~CopyCommandList() = default;

void FWK::Graphics::CopyCommandList::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION& a_destination,
													   const D3D12_TEXTURE_COPY_LOCATION& a_source,
													   const D3D12_BOX*					  a_sourceBox,
													   const UINT						  a_destinationX,
													   const UINT						  a_destinationY,
													   const UINT						  a_destinationZ) const
{
	const auto& l_copyCommandList = GetREFCommandList();

	if (!l_copyCommandList)
	{
		assert(false && "コピーコマンドリストが作成されておらず、テクスチャコピー処理に失敗しました。");
		return;
	}

	// D3D12_TEXTURE_COPY_LOCATION構造体についての説明
	// pResource : コピー先またはコピー元になるリソース
	// Type      : どの方法でコピー位置を指定するか
	//			　 SUBRESOURCE_INDEXなら「何番目のサブリソースか」
	//			   PLACED_FOOTPRINTなら「UploadBuffer上のどこに画像があるか」

	// どのリソースのどこへ、どのリソースのどこからコピーするかを指定する
	// CopyTextureRegion(コピー先の情報、
	//					 コピー先X座標、
	//					 コピー先Y座標、
	//					 コピー先Z座標,
	//					 コピー元の情報、
	//					 コピー元範囲(nullptrなら全体));

	l_copyCommandList->CopyTextureRegion(&a_destination,
										 a_destinationX,
										 a_destinationY,
										 a_destinationZ,
										 &a_source,
										 a_sourceBox);
}

void FWK::Graphics::CopyCommandList::CopyBufferRegion(const TypeAlias::ComPtr<ID3D12Resource2>& a_destinationBuffer,
													  const TypeAlias::ComPtr<ID3D12Resource2>& a_sourceBuffer, 
													  const UINT64&								a_destinationOffset, 
												      const UINT64&								a_sourceOffset,
													  const UINT64&								a_bufferSize) const
{
	const auto& l_copyCommandList = GetREFCommandList();

	if (!l_copyCommandList)
	{
		assert(false && "コピーコマンドリストが作成されておらず、バッファコピー処理に失敗しました。");
		return;
	}

	if (!a_destinationBuffer)
	{
		assert(false && "コピー先BufferResourceが無効のため、バッファコピー処理に失敗しました。");
		return;
	}

	if (!a_sourceBuffer)
	{
		assert(false && "コピー元UploadBufferが無効のため、バッファコピー処理に失敗しました。");
		return;
	}

	if (a_bufferSize == Constant::k_invalidBufferSize)
	{
		assert(false && "コピーするBufferサイズが0のため、バッファコピー処理に失敗しました。");
		return;
	}

	// CopyBufferRegion(コピー先BufferResource、
	//					コピー先BufferResource内の書き込み開始Offset、
	//					コピー元UploadBuffer、
	//					コピー元UploadBuffer内の読み取り開始Offset、
	//					コピーするByteSize);
	l_copyCommandList->CopyBufferRegion(a_destinationBuffer.Get(),
										a_destinationOffset,
										a_sourceBuffer.Get(),
										a_sourceOffset,
										a_bufferSize);
}