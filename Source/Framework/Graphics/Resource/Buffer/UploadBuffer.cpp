#include "UploadBuffer.h"

bool FWK::Graphics::UploadBuffer::Create(const UINT64& a_bufferSize, const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらず、UploadBufferの作成に失敗しました。");
		return false;
	}

	// サイズ0のバッファは作成する意味がないため失敗扱いにする
	if (a_bufferSize == k_invalidBufferSize)
	{
		assert(false && "UploadBufferの作成サイズが0のため、UploadBufferの作成に失敗しました。");
		return false;
	}

	constexpr auto l_nodeMask = GraphicsManager::GetVALDefaultGPUNodeMask();

	// D3D12_HEAP_PROPERTIESについての説明
	// Type                 : どの種類のヒープにリソースを置くか
	//		                  D3D12_HEAP_TYPE_UPLOADなのでCPUからMapして書き込むことを想定したアップロード用ヒープ
	// CPUPageProperty		: CPUページ属性(既定値を使うため内部的にUNKNOWN相当)
	// MemoryPoolPreference : メモリプール指定(既定値を使うため内部的にUNKNOWN相当)
	// CreationNodeMask		: 作製対象GPUノード
	// VisibleNodeMask	    : 可視対象GPUノード
	// UploadBufferはCPUでデータを書き込み、
	// その後CopyBufferRegionやCopyTextureRegionで本番リソースへ転送するため、
	// D3D12_HEAP_TYPE_UPLOADで作成する
	auto l_heapProperties = CD3DX12_HEAP_PROPERTIES(k_uploadBufferHeapType, l_nodeMask, l_nodeMask);

	// D3D12_RESOURCE_DESCについての説明
	// Buffer(作成するバッファサイズ)
	// 今回はテクスチャではなく単純な線形メモリとして扱うアップロード用バッファを作成する。
	auto l_resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(a_bufferSize);

	// CreateCommittedResource(ヒープ設定、
	//						   ヒープ追加フラグ、
	//						   リソース設定、
	//						   初期リソース状態、
	//						   ClearValue(nullptrなので未使用)、
	//						   受け取りたいインターフェース型、
	//						   作成結果のポインタ書き込み先);
	// CommittedResourceはヒープとリソースをまとめて一つで作成する方式
	// UploadBufferは転送用の一時バッファであり、管理を複雑にしなくてよいためこの方式で十分
	// 初期状態にD3D12_RESOURCE_STATE_GENERIC_READを指定している理由
	// UploadHeap上のリソースはCPUから書き込み、GPUから読み取ってコピー元として使う想定のため
	const auto l_hr = l_device->CreateCommittedResource(&l_heapProperties,
														k_defaultUploadBufferHeapFlags,
													    &l_resourceDesc,
														k_uploadBufferInitialResourceState,
														nullptr,
														IID_PPV_ARGS(m_uploadBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "UploadBufferの作成に失敗しました。");
		return false;
	}

	return true;
}

std::uint8_t* FWK::Graphics::UploadBuffer::Map() const
{
	if (!m_uploadBuffer)
	{
		assert(false && "UploadBufferが作成されておらず、Mapに失敗しました。");
		return nullptr;
	}

	// Map成功時にCPUから書き込める先頭アドレスを受け取る
	void* l_mappedData = nullptr;

	// Map(対象サブリソース番号、
	//	   CPUが読む範囲情報(nullptrなので範囲指定なし)、
	//	   マップした先頭アドレスの受取先);
	
	// Bufferリソースなのでサブリソースは0固定で扱う
	// UploadBufferはCPUから書き込みたい用途なのでMapして生ポインタを取得する
	const auto l_hr = m_uploadBuffer->Map(k_bufferSubresourceIndex, nullptr, &l_mappedData);

	if (FAILED(l_hr))
	{
		assert(false && "UploadBufferのMapに失敗しました。");
		return nullptr;
	}

	// byte単位でオフセット計算しやすいようstd::uint8_t*へ変換して返す
	return static_cast<std::uint8_t*>(l_mappedData);
}
void FWK::Graphics::UploadBuffer::UnMap() const
{
	if (!m_uploadBuffer)
	{
		assert(false && "UploadBufferが作成されておらず、UnMapに失敗しました。");
		return;
	}

	// Unmap(対象サブリソース番号、
	//		 CPUが書き込んだ範囲情報(nullptrなので範囲指定なし));

	// Bufferリソースなのでサブリソースは0固定
	// 今回は書き込み範囲を明示していないためnullptrを渡す
	m_uploadBuffer->Unmap(k_bufferSubresourceIndex, nullptr);
}