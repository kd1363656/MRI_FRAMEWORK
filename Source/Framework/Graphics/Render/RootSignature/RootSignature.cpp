#include "RootSignature.h"

void FWK::Graphics::RootSignature::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }
	m_rootSignatureJsonConverter.Deserialize(a_rootJson, *this);
}
bool FWK::Graphics::RootSignature::Create(const Device& a_device)
{
	const auto& l_device = a_device.GetREFDevice();

	if (!l_device)
	{
		assert(false && "デバイスが作成されておらずルートシグネチャを作成できませんでした。");
		return false;
	}

	// ルートパラメータのみを格納するリストを作成
	std::vector<D3D12_ROOT_PARAMETER> l_rootParameterList = {};

	// ルートパラメータの数分予約しておく
	l_rootParameterList.reserve(m_rootParameterRecordList.size());
	
	// std::vectorで管理しているリストは
	// reserveなどであらかじめに確保する予定である要素数を指定しなければ
	// 要素の再確保が発生し、ポインタが無効になるため全てを追加してからポインタを結び付ける
	std::transform(m_rootParameterRecordList.begin(),
				   m_rootParameterRecordList.end(),
				   std::back_inserter(l_rootParameterList),
				   [](const Struct::RootParameterRecord& a_rootParameterRecord)
				   {
						return a_rootParameterRecord.m_rootParameter;
				   });

	// ルートシグネチャ全体の設定
	D3D12_ROOT_SIGNATURE_DESC l_desc = {};

	// ルートパラメータ数
	l_desc.NumParameters = static_cast<UINT>(l_rootParameterList.size());

	// ルートパラメータ配列先頭
	l_desc.pParameters = l_rootParameterList.data();

	// StaticSampler数
	l_desc.NumStaticSamplers = static_cast<UINT>(m_staticSamplerDescList.size());

	// StaticSampler配列先頭
	l_desc.pStaticSamplers = m_staticSamplerDescList.data();

	// オプションフラグ
	l_desc.Flags = m_rootSignatureFlags;

	TypeAlias::ComPtr<ID3DBlob> l_serializedBlob = nullptr;
	TypeAlias::ComPtr<ID3DBlob> l_errorBlob      = nullptr;

	// ルートシグネチャの設定情報をシリアライズする
	// D3D12SerializeRootSignature(ルートシグネチャの設定内容、
	//							   どのバージョンのルートシグネチャとしてシリアライズするか、
	//							   シリアライズ後のバイナリデータを受け取り先、
	//							   エラー時のメッセージ受け取り先);

	// DirectX12では、設定構造体をそのままCreateRootSignatureに渡すのではなく、
	// まずシリアライズしてバイナリ化したデータを作る必要がある
	auto l_hr = D3D12SerializeRootSignature(&l_desc,
											m_rootSignatureVersion,
											l_serializedBlob.ReleaseAndGetAddressOf(),
											l_errorBlob.ReleaseAndGetAddressOf());

	if (FAILED(l_hr))
	{
		// シリアライズ失敗時、詳細なエラーメッセージがあればデバッグ出力する
		if (l_errorBlob)
		{
			OutputDebugStringA((char*)l_errorBlob->GetBufferPointer());
		}

		assert(false && "ルートシグネチャのシリアライズに失敗しました。");
		return false;
	}

	// シリアライズ済みバイナリから実際のルートシグネチャオブジェクトを作成する
	// CreateRootSignature(作成対象GPUノード番号、
	//					   シリアライズ済みルートシグネチャデータの先頭アドレス、
	//					   シリアライズ済みデータのサイズ、
	//					   受け取りたいCOMインターフェース型のID、
	//					   作成結果のポインタを書き込むアドレス);

	l_hr = l_device->CreateRootSignature(GraphicsManager::GetVALDefaultGPUNodeMask(),
										 l_serializedBlob->GetBufferPointer(),
										 l_serializedBlob->GetBufferSize(),
										 IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));

	if (FAILED(l_hr))
	{
		assert(false && "ルートシグネチャの作成に失敗しました。");
		return false;
	}

	return true;
}

nlohmann::json FWK::Graphics::RootSignature::Serialize() const
{
	return m_rootSignatureJsonConverter.Serialize(*this);
}

UINT FWK::Graphics::RootSignature::FindVALRootParameterIndex(const TypeAlias::TypeTag a_tag) const
{
	const auto& l_itr = m_rootParameterIndexMap.find(a_tag);

	if (l_itr == m_rootParameterIndexMap.end()) { return Constant::k_invalidRootParameterIndex; }

	return l_itr->second;
}