#include "RootSignatureJsonConverter.h"

void FWK::JsonConverter::RootSignatureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null()) { return; }

	// タグに対応したルートパラメータアクセス用インデックスを読み込む
	if (Utility::Json::IsArray(a_rootJson, "RootParameterIndexMap"))
	{
		DeserializeRootParameterIndexMap(a_rootJson["RootParameterIndexMap"], a_rootSignature);
	}

	// ルートパラメータを読み込む
	if (Utility::Json::IsArray(a_rootJson, "RootParameterList"))
	{
		DeserializeRootParameterList(a_rootJson["RootParameterList"], a_rootSignature);
	}

	// StaticSamplerDescを読み込む
	if (Utility::Json::IsArray(a_rootJson, "StaticSamplerDescList"))
	{
		DeserializeStaticSamplerDescList(a_rootJson["StaticSamplerDescList"], a_rootSignature);
	}

	// このルートシグネチャをパイプラインからどう使うかを決定するフラグ
	// どのシェーダーステージからアクセスするか、InputAssemblerを使うかLocalRootSignatureかを決める
	const auto l_flags = a_rootJson.value("RootSignatureFlags", D3D12_ROOT_SIGNATURE_FLAG_NONE);

	// どのバージョンのルートシグネチャ仕様でシリアライズするかを決める
	const auto l_version = a_rootJson.value("RootSignatureVersion", D3D_ROOT_SIGNATURE_VERSION_1);

	a_rootSignature.SetRootSignatureFlags  (l_flags);
	a_rootSignature.SetRootSignatureVersion(l_version);
}
nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::Serialize(const Graphics::RootSignature& a_rootSignature) const
{
	nlohmann::json l_rootJson = {};

	// ルートパラメータインデックス、ルートパラメータリスト、
	l_rootJson["RootParameterIndexMap"] = SerializeRootParameterIndexMap(a_rootSignature);
	l_rootJson["RootParameterList"]     = SerializeRootParameterList    (a_rootSignature);
	l_rootJson["StaticSamplerDescList"] = SerializeStaticSamplerDescList(a_rootSignature);

	l_rootJson["RootSignatureFlags"]   = a_rootSignature.GetVALRootSignatureFlags  ();
	l_rootJson["RootSignatureVersion"] = a_rootSignature.GetVALRootSignatureVersion();
	
	return l_rootJson;
}

void FWK::JsonConverter::RootSignatureJsonConverter::DeserializeRootParameterIndexMap(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }
	
	auto& l_rootParameterIndexMap = a_rootSignature.GetMutableREFRootParameterIndexMap();

	// 要素が既にある可能性を考慮してClear
	l_rootParameterIndexMap.clear();

	for (const auto& l_json : a_rootJson)
	{
		const auto l_tag   = Utility::Json::DeserializeTag(l_json, "RootParameterTag");
		const auto l_index = l_json.value                 ("Index", Constant::k_invalidRootParameterIndex);

		l_rootParameterIndexMap.try_emplace(l_tag, l_index);
	}
}
void FWK::JsonConverter::RootSignatureJsonConverter::DeserializeRootParameterList(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }
	
	const auto& l_jsonArraySize		      = a_rootJson.size								        ();
		  auto& l_rootParameterRecordList = a_rootSignature.GetMutableREFRootParameterRecordList();

	// 要素が既にある可能性を考慮してClear
	l_rootParameterRecordList.clear();

	// まずは配列の容量を決める
	l_rootParameterRecordList.resize(l_jsonArraySize);

	// リサイズをjson配列の大きさで行っているのでif文で配列外かどうかのインデックス確認を行わない
	// json内部で保存されているデータをl_rootParameterListにパースする
	for (size_t l_i = 0ULL; l_i < l_jsonArraySize; ++l_i)
	{
		const auto& l_json = a_rootJson[l_i];
		
		// ルートパラメータインデックスを取得するためのタグを格納
		l_rootParameterRecordList[l_i].m_rootParameterTag = Utility::Json::DeserializeTag(l_json, "RootParameterTag");

		auto& l_rootParameterRecord = l_rootParameterRecordList[l_i].m_rootParameter;

		// ルートパラメータの種類、シェーダー可視性を格納
		l_rootParameterRecord.ParameterType    = l_json.value("ParameterType",    D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		l_rootParameterRecord.ShaderVisibility = l_json.value("ShaderVisibility", D3D12_SHADER_VISIBILITY_ALL);

		switch (l_rootParameterRecord.ParameterType)
		{
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				DeserializeDescriptorRangeList(l_json, l_rootParameterRecordList[l_i]);
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			{
				// b番号担当のシェーダーレジスタ番号
				l_rootParameterRecord.Constants.ShaderRegister = l_json.value("ShaderRegister", k_defaultShaderRegister);

				// レジスタ空間
				l_rootParameterRecord.Constants.RegisterSpace = l_json.value("RegisterSpace", k_defaultRegisterSpace);

				// 32bit値の個数
				l_rootParameterRecord.Constants.Num32BitValues = l_json.value("Num32BitValues", k_defaultRootConstantsNum32BitValues);
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
			{
				// b / t/ u 番号
				l_rootParameterRecord.Descriptor.ShaderRegister = l_json.value("ShaderRegister", k_defaultShaderRegister);

				// レジスタ空間
				l_rootParameterRecord.Descriptor.RegisterSpace = l_json.value("RegisterSpace", k_defaultRegisterSpace);
			}
			break;

			default:
			{
				assert(false && "未対応のRootParameterTypeが指定されました。");
				return;
			}
		}
	}
}
void FWK::JsonConverter::RootSignatureJsonConverter::DeserializeStaticSamplerDescList(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null())				 { return; }
	if (!Utility::Json::IsArray(a_rootJson)) { return; }
	
	auto& l_staticSamplerDescList = a_rootSignature.GetMutableREFStaticSamplerDescList();

	const auto& l_jsonArraySize = a_rootJson.size();

	// 要素が既にある可能性を考慮してClear
	l_staticSamplerDescList.clear();

	// StaticSamplerDescをjsonが保持している数分容量を確保する
	l_staticSamplerDescList.resize(l_jsonArraySize);

	// リサイズをjson配列の大きさで行っているのでif文で配列外かどうかのインデックス確認を行わない
	for (size_t l_i = 0ULL; l_i < l_jsonArraySize; ++l_i)
	{
		const auto& l_json			    = a_rootJson			 [l_i];
			  auto& l_staticSamplerDesc = l_staticSamplerDescList[l_i];

		// サンプリング方法
		l_staticSamplerDesc.Filter = l_json.value("Filter", D3D12_FILTER_MIN_MAG_MIP_POINT);
		
		// U方向のアドレスモード
		l_staticSamplerDesc.AddressU = l_json.value("AddressU", D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		
		// V方向のアドレスモード
		l_staticSamplerDesc.AddressV = l_json.value("AddressV", D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		// W方向のアドレスモード
		// 2Dテクスチャでは直接使わないことが多いが、基本形として合わせて設定する
		l_staticSamplerDesc.AddressW = l_json.value("AddressW", D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		// MIPレベル計算時の補正値
		l_staticSamplerDesc.MipLODBias = l_json.value("MipLODBias", k_defaultStaticSamplerMIPLODBias);
		
		// 異方性フィルタ使用時の最大サンプル数
		l_staticSamplerDesc.MaxAnisotropy = l_json.value("MaxAnisotropy", k_defaultStaticSamplerMAXAnisotropy);
		
		// 比較サンプラーで使う比較関数
		l_staticSamplerDesc.ComparisonFunc = l_json.value("ComparisonFunc", D3D12_COMPARISON_FUNC_ALWAYS);

		// アドレスモード使用時の境界色の色
		l_staticSamplerDesc.BorderColor = l_json.value("BorderColor", D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
		
		// 参照可能な最小MIPレベル
		l_staticSamplerDesc.MinLOD = l_json.value("MinLOD", k_defaultMINLOD);
		
		// 参照可能な最大MIPレベル
		l_staticSamplerDesc.MaxLOD = l_json.value("MaxLOD", D3D12_FLOAT32_MAX);
		
		// シェーダー側のSamplerRegister番号
		l_staticSamplerDesc.ShaderRegister = l_json.value("ShaderRegister", k_defaultShaderRegister);

		// Sampler用のRegisterSpace
		l_staticSamplerDesc.RegisterSpace = l_json.value("RegisterSpace", k_defaultRegisterSpace);
		
		// このSamplerをどのシェーダーステージから見えるようにするか
		l_staticSamplerDesc.ShaderVisibility = l_json.value("ShaderVisibility", D3D12_SHADER_VISIBILITY_PIXEL);
	}
}

nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeRootParameterIndexMap(const Graphics::RootSignature& a_rootSignature) const
{
	nlohmann::json l_rootJsonArray = nlohmann::json::array();
	
	const auto& l_rootParameterIndexMap = a_rootSignature.GetREFRootParameterIndexMap();

	// タグとルートインデックスを対応付けて保存
	for (const auto& [l_key, l_value] : l_rootParameterIndexMap)
	{
		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_key, "RootParameterTag"));

		l_json["Index"] = l_value;

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}
nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeRootParameterList(const Graphics::RootSignature& a_rootSignature) const
{
	auto l_jsonArray = nlohmann::json::array();

	const auto& l_rootParameterList = a_rootSignature.GetREFRootParameterRecordList();

	// ルートパラメータの配列を保存
	for (const auto& l_rootParameter : l_rootParameterList)
	{
		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_rootParameter.m_rootParameterTag, "RootParameterTag"));

		l_json["ParameterType"]    = l_rootParameter.m_rootParameter.ParameterType;
		l_json["ShaderVisibility"] = l_rootParameter.m_rootParameter.ShaderVisibility;

		switch(l_rootParameter.m_rootParameter.ParameterType)
		{
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				l_json["DescriptorRangeList"] = SerializeDescriptorRangeList(l_rootParameter);
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			{
				l_json["ShaderRegister"] = l_rootParameter.m_rootParameter.Constants.ShaderRegister;
				l_json["RegisterSpace"]  = l_rootParameter.m_rootParameter.Constants.RegisterSpace;
				l_json["Num32BitValues"] = l_rootParameter.m_rootParameter.Constants.Num32BitValues;
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
			{
				l_json["ShaderRegister"] = l_rootParameter.m_rootParameter.Descriptor.ShaderRegister;
				l_json["RegisterSpace"]  = l_rootParameter.m_rootParameter.Descriptor.RegisterSpace;
			}
			break;

			default:
			{
				assert(false && "未対応のRootParameterTypeが指定されました。");
				return {};
			}
		}
		
		l_jsonArray.emplace_back(l_json);
	}

	return l_jsonArray;
}
nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeStaticSamplerDescList(const Graphics::RootSignature& a_rootSignature) const
{
	auto l_rootJsonArray = nlohmann::json::array();
	
	const auto& l_staticSamplerDescList = a_rootSignature.GetREFStaticSamplerDescList();

	// スタティックサンプラーの設定リストを保存
	for (const auto& l_staticSamplerDesc : l_staticSamplerDescList)
	{
		nlohmann::json l_json = {};

		// サンプリング方法
		l_json["Filter"] = l_staticSamplerDesc.Filter;
		
		// U方向のアドレスモード
		l_json["AddressU"] = l_staticSamplerDesc.AddressU;
		
		// V方向のアドレスモード
		l_json["AddressV"] = l_staticSamplerDesc.AddressV;

		// W方向のアドレスモード
		// 2Dテクスチャでは直接使わないことが多いが、基本形として合わせて設定する
		l_json["AddressW"] = l_staticSamplerDesc.AddressW;

		// MIPレベル計算時の補正値
		l_json["MipLODBias"] = l_staticSamplerDesc.MipLODBias;
		
		// 異方性フィルタ使用時の最大サンプル数
		l_json["MaxAnisotropy"] = l_staticSamplerDesc.MaxAnisotropy;
		
		// 比較サンプラーで使う比較関数
		l_json["ComparisonFunc"] = l_staticSamplerDesc.ComparisonFunc;

		// アドレスモード使用時の境界色の色
		l_json["BorderColor"] = l_staticSamplerDesc.BorderColor;
		
		// 参照可能な最小MIPレベル
		l_json["MinLOD"] = l_staticSamplerDesc.MinLOD;
		
		// 参照可能な最大MIPレベル
		l_json["MaxLOD"] = l_staticSamplerDesc.MaxLOD;
		
		// シェーダー側のSamplerRegister番号
		l_json["ShaderRegister"] = l_staticSamplerDesc.ShaderRegister;

		// Sampler用のRegisterSpace
		l_json["RegisterSpace"] = l_staticSamplerDesc.RegisterSpace;
		
		// このSamplerをどのシェーダーステージから見えるようにするか
		l_json["ShaderVisibility"] = l_staticSamplerDesc.ShaderVisibility;

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}

void FWK::JsonConverter::RootSignatureJsonConverter::DeserializeDescriptorRangeList(const nlohmann::json& a_rootJson, Struct::RootParameterRecord& a_rootParameterRecord) const
{
	// jsonに保存されていなかったということはD3D12_DESCRIPTOR_RANGEは使わなかったということなので
	// NumDescriptorRangesは0になる
	if (a_rootJson.is_null())										
	{
		// 明示的にディスクリプタテーブルを使用しないように初期値を格納
		a_rootParameterRecord.m_rootParameter.DescriptorTable.NumDescriptorRanges = k_invalidNUMDescriptorRange;
		a_rootParameterRecord.m_rootParameter.DescriptorTable.pDescriptorRanges   = nullptr;
		return; 
	}

	if (!Utility::Json::IsArray(a_rootJson, "DescriptorRangeList")) 
	{
		// 明示的にディスクリプタテーブルを使用しないように初期値を格納
		a_rootParameterRecord.m_rootParameter.DescriptorTable.NumDescriptorRanges = k_invalidNUMDescriptorRange;
		a_rootParameterRecord.m_rootParameter.DescriptorTable.pDescriptorRanges   = nullptr;
		return; 
	}

	auto& l_descriptorRangeList = a_rootParameterRecord.m_descriptorRangeList;

	// もしインスタンス化されていなければする
	if (!l_descriptorRangeList)
	{
		l_descriptorRangeList = std::make_shared<std::vector<D3D12_DESCRIPTOR_RANGE>>();
	}

	const auto& l_jsonArray = a_rootJson["DescriptorRangeList"];

	const auto& l_jsonArraySize = l_jsonArray.size();

	// 要素が既にある可能性を考慮してClear
	l_descriptorRangeList->clear();

	// ディスクリプタレンジをl_jsonArrayが持っているデータの個数分要素を確保
	l_descriptorRangeList->resize(l_jsonArraySize);

	// リサイズをjson配列の大きさで行っているのでif文で配列外かどうかのインデックス確認を行わない
	for (size_t l_i = 0ULL; l_i < l_jsonArraySize; ++l_i)
	{
		auto& l_descriptorRange = (*l_descriptorRangeList)[l_i];

		// このレンジに入るディスクリプタの種類
		l_descriptorRange.RangeType = l_jsonArray[l_i].value("RangeType", D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

		// このレンジに含めるディスクリプタ数
		l_descriptorRange.NumDescriptors = l_jsonArray[l_i].value("NumDescriptors", k_defaultDescriptorRangeNUMDescriptors);

		// シェーダーレジスタの開始番号
		l_descriptorRange.BaseShaderRegister = l_jsonArray[l_i].value("BaseShaderRegister", k_defaultBaseShaderRegister);

		// レジスタ空間
		l_descriptorRange.RegisterSpace = l_jsonArray[l_i].value("RegisterSpace", k_defaultRegisterSpace);

		// ディスクリプタテーブル先頭からのオフセット
		l_descriptorRange.OffsetInDescriptorsFromTableStart = l_jsonArray[l_i].value("OffsetInDescriptorsFromTableStart", D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	}

	// ディスクリプタテーブルで使用するディスクリプタレンジの数
	a_rootParameterRecord.m_rootParameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(l_descriptorRangeList->size());

	// ディスクリプタテーブルで参照するディスクリプタレンジのポインタ
	a_rootParameterRecord.m_rootParameter.DescriptorTable.pDescriptorRanges = l_descriptorRangeList->data();
}

nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeDescriptorRangeList(const Struct::RootParameterRecord& a_rootParameterRecord) const
{
	// ディスクリプタレンジリストが存在しなければreturn
	if (!a_rootParameterRecord.m_descriptorRangeList) { return {}; }

	nlohmann::json l_rootJsonArray = nlohmann::json::array();
	
	// ディスクリプタレンジの内容をjsonに保存
	for (const auto& l_descriptorRange : *a_rootParameterRecord.m_descriptorRangeList)
	{
		nlohmann::json l_json = {};

		l_json["RangeType"]                         = l_descriptorRange.RangeType;
		l_json["NumDescriptors"]                    = l_descriptorRange.NumDescriptors;
		l_json["BaseShaderRegister"]                = l_descriptorRange.BaseShaderRegister;
		l_json["RegisterSpace"]                     = l_descriptorRange.RegisterSpace;
		l_json["OffsetInDescriptorsFromTableStart"] = l_descriptorRange.OffsetInDescriptorsFromTableStart;

		l_rootJsonArray.emplace_back(l_json);
	}

	return l_rootJsonArray;
}