#include "RootSignatureJsonConverter.h"

void FWK::JsonConverter::RootSignatureJsonConverter::Deserialize(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null()) { return; }

	// タグに対応したルートパラメータアクセス用インデックスを読み込む
	DeserializeRootParameterIndexMap(a_rootJson, a_rootSignature);

	// ルートパラメータを読み込む
	DeserializeRootParameterList(a_rootJson, a_rootSignature);

	// StaticSamplerDescを読み込む
	DeserializeStaticSamplerDescList(a_rootJson, a_rootSignature);

	// このルートシグネチャをパイプラインからどう使うかを決定するフラグ
	// 度のシェーダーステージからアクセスするか、InputAssemblerを使うかLocalRootSignatureかを決める
	const auto l_flags   = a_rootJson.value("RootSignatureFlags", D3D12_ROOT_SIGNATURE_FLAG_NONE);

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
	if (a_rootJson.is_null())										  { return; }
	if (!Utility::Json::IsArray(a_rootJson, "RootParameterIndexMap")) { return; }
	
	auto& l_rootParameterIndexMap = a_rootSignature.GetMutableREFRootParameterIndexMap();

	// 要素が既にある可能性を考慮してClear
	l_rootParameterIndexMap.clear();

	for (const auto& l_json : a_rootJson["RootParameterIndexMap"])
	{
		const auto l_tag   = Utility::Json::DeserializeTag(l_json, "RootParameterTag");
		const auto l_index = l_json.value                 ("Index", Constant::k_invalidRootParameterIndex);

		l_rootParameterIndexMap.try_emplace(l_tag, l_index);
	}
}
void FWK::JsonConverter::RootSignatureJsonConverter::DeserializeRootParameterList(const nlohmann::json& a_rootJson, Graphics::RootSignature& a_rootSignature) const
{
	if (a_rootJson.is_null())									  { return; }
	if (!Utility::Json::IsArray(a_rootJson, "RootParameterList")) { return; }
	
	const auto& l_jsonArray         = a_rootJson["RootParameterList"];
	const auto& l_jsonArraySize		= l_jsonArray.size();
		  auto& l_rootParameterList = a_rootSignature.GetMutableREFRootParameterList();

	// 要素が既にある可能性を考慮してClear
	l_rootParameterList.clear();

	// まずは配列の容量を決める
	l_rootParameterList.resize(l_jsonArraySize);

	// リサイズをjson配列の大きさで行っているのでif文で配列外かどうかのインデックス確認を行わない
	// json内部で保存されているデータをl_rootParameterListにパースする
	for (size_t l_i = 0ULL; l_i < l_jsonArraySize; ++l_i)
	{
		const auto& l_json = l_jsonArray[l_i];
		
		// ルートパラメータインデックスを取得するためのタグを格納
		l_rootParameterList[l_i].rootParameterTag = Utility::Json::DeserializeTag(l_json, "RootParameterTag");

		auto& l_rootParameter = l_rootParameterList[l_i].rootParameter;

		// ルートパラメータの種類、シェーダー可視性を格納
		l_rootParameter.ParameterType    = l_json.value("ParameterType",    D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE);
		l_rootParameter.ShaderVisibility = l_json.value("ShaderVisibility", D3D12_SHADER_VISIBILITY_ALL);

		switch (l_rootParameter.ParameterType)
		{
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				DeserializeDescriptorRangeList(l_json, l_rootParameterList[l_i]);
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			{
				// b番号担当のシェーダーレジスタ番号
				l_rootParameter.Constants.ShaderRegister = l_json.value("ShaderRegister", 0U);

				// レジスタ空間
				l_rootParameter.Constants.RegisterSpace = l_json.value("RegisterSpace", 0U);

				// 32bit値の個数
				l_rootParameter.Constants.Num32BitValues = l_json.value("Num32BitValues", 0U);
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
			{
				// b / t/ u 番号
				l_rootParameter.Descriptor.ShaderRegister = l_json.value("ShaderRegister", 0U);

				// レジスタ空間
				l_rootParameter.Descriptor.RegisterSpace = l_json.value("RegisterSpace", 0U);
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
	if (a_rootJson.is_null())									      { return; }
	if (!Utility::Json::IsArray(a_rootJson, "StaticSamplerDescList")) { return; }
	
	auto& l_staticSamplerDescList = a_rootSignature.GetMutableREFStaticSamplerDescList();

	const auto& l_jsonArray     = a_rootJson["StaticSamplerDescList"];
	const auto& l_jsonArraySize = l_jsonArray.size();

	// 要素が既にある可能性を考慮してClear
	l_staticSamplerDescList.clear();

	// StaticSamplerDescをjsonが保持している数分容量を確保する
	l_staticSamplerDescList.resize(l_jsonArraySize);

	// リサイズをjson配列の大きさで行っているのでif文で配列外かどうかのインデックス確認を行わない
	for (size_t l_i = 0ULL; l_i < l_jsonArraySize; ++l_i)
	{
		const auto& l_json			    = l_jsonArray			 [l_i];
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
		l_staticSamplerDesc.MipLODBias = l_json.value("MipLODBias", 0.0F);
		
		// 異方性フィルタ使用時の最大サンプル数
		l_staticSamplerDesc.MaxAnisotropy = l_json.value("MaxAnisotropy", 1U);
		
		// 比較サンプラーで使う比較関数
		l_staticSamplerDesc.ComparisonFunc = l_json.value("ComparisonFunc", D3D12_COMPARISON_FUNC_ALWAYS);

		// アドレスモード使用時の境界色の色
		l_staticSamplerDesc.BorderColor = l_json.value("BorderColor", D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
		
		// 参照可能な最小MIPレベル
		l_staticSamplerDesc.MinLOD = l_json.value("MinLOD", 0.0F);
		
		// 参照可能な最大MIPレベル
		l_staticSamplerDesc.MaxLOD = l_json.value("MaxLOD", D3D12_FLOAT32_MAX);
		
		// シェーダー側のSamplerRegister番号
		l_staticSamplerDesc.ShaderRegister = l_json.value("ShaderRegister", 0U);

		// Sampler用のRegisterSpace
		l_staticSamplerDesc.RegisterSpace = l_json.value("RegisterSpace", 0U);
		
		// このSamplerをどのシェーダーステージから見えるようにするか
		l_staticSamplerDesc.ShaderVisibility = l_json.value("ShaderVisibility", D3D12_SHADER_VISIBILITY_PIXEL);
	}
}

nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeRootParameterIndexMap(const Graphics::RootSignature& a_rootSignature) const
{
	nlohmann::json l_rootJson  = {};
	auto		   l_jsonArray = nlohmann::json::array();

	const auto& l_rootParameterIndexMap = a_rootSignature.GetREFRootParameterIndexMap();

	// タグとルートインデックスを対応付けて保存
	for (const auto& [l_key, l_value] : l_rootParameterIndexMap)
	{
		nlohmann::json l_json = {};

		Utility::Json::UpdateJson(l_json, Utility::Json::SerializeTag(l_key, "RootParameterTag"));

		l_json["Index"] = l_value;

		l_jsonArray.emplace_back(l_json);
	}

	l_rootJson["RootParameterIndexMap"] = l_jsonArray;

	return l_rootJson;
}
nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeRootParameterList(const Graphics::RootSignature& a_rootSignature) const
{
	nlohmann::json l_rootJson  = {};
	auto		   l_jsonArray = nlohmann::json::array();

	const auto& l_rootParameterList = a_rootSignature.GetREFRootParameterList();

	// ルートパラメータの配列を保存
	for (const auto& l_rootParameter : l_rootParameterList)
	{
		nlohmann::json l_json = {};

		switch(l_rootParameter.rootParameter.ParameterType)
		{
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			{
				l_rootJson["DescriptorRangeList"] = SerializeDescriptorRangeList(l_rootParameter);
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			{
				l_rootJson["ShaderRegister"] = l_rootParameter.rootParameter.Constants.ShaderRegister;
				l_rootJson["RegisterSpace"]  = l_rootParameter.rootParameter.Constants.RegisterSpace;
				l_rootJson["Num32BitValues"] = l_rootParameter.rootParameter.Constants.Num32BitValues;
			}
			break;

			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
			{
				l_rootJson["ShaderRegister"] = l_rootParameter.rootParameter.Descriptor.ShaderRegister;
				l_rootJson["RegisterSpace"]  = l_rootParameter.rootParameter.Descriptor.RegisterSpace;
			}
			break;

			default:
			{
				assert(false && "未対応のRootParameterTypeが指定されました。");
				return l_rootJson;
			}
		}
		
		l_jsonArray.emplace_back(l_json);
	}

	l_rootJson["RootParameterList"] = l_jsonArray;

	return l_rootJson;
}
nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeStaticSamplerDescList(const Graphics::RootSignature& a_rootSignature) const
{
	nlohmann::json l_rootJson  = {};
	auto		   l_jsonArray = nlohmann::json::array();

	const auto& l_staticSamplerDescList = a_rootSignature.GetREFStaticSamplerDescList();

	// タグとルートインデックスを対応付けて保存
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
		l_rootJson["MinLOD"] = l_staticSamplerDesc.MinLOD;
		
		// 参照可能な最大MIPレベル]
		l_json["MaxLOD"] = l_staticSamplerDesc.MaxLOD;
		
		// シェーダー側のSamplerRegister番号
		l_json["ShaderRegister"] = l_staticSamplerDesc.ShaderRegister;

		// Sampler用のRegisterSpace
		l_json["RegisterSpace"] = l_staticSamplerDesc.RegisterSpace;
		
		// このSamplerをどのシェーダーステージから見えるようにするか
		l_json["ShaderVisibility"] = l_staticSamplerDesc.ShaderVisibility;

		l_jsonArray.emplace_back(l_json);
	}

	l_rootJson["StaticSamplerDescList"] = l_jsonArray;

	return l_rootJson;
}

void FWK::JsonConverter::RootSignatureJsonConverter::DeserializeDescriptorRangeList(const nlohmann::json& a_rootJson, Struct::RootParameter& a_rootParameter) const
{
	// jsonに保存されていなかったということはD3D12_DESCRIPTOR_RANGEは使わなかったということなので
	// NumDescriptorRangesは0になる
	if (a_rootJson.is_null())										
	{
		a_rootParameter.rootParameter.DescriptorTable.NumDescriptorRanges = 0U;
		a_rootParameter.rootParameter.DescriptorTable.pDescriptorRanges   = nullptr;
		return; 
	}

	if (!Utility::Json::IsArray(a_rootJson, "DescriptorRangeList")) 
	{
		a_rootParameter.rootParameter.DescriptorTable.NumDescriptorRanges = 0U;
		a_rootParameter.rootParameter.DescriptorTable.pDescriptorRanges   = nullptr;
		return; 
	}

	auto& l_descriptorRangeList = a_rootParameter.descriptorRangeList;

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
		l_descriptorRange.NumDescriptors = l_jsonArray[l_i].value("NumDescriptors", 0U);

		// シェーダーレジスタの開始番号
		l_descriptorRange.BaseShaderRegister = l_jsonArray[l_i].value("BaseShaderRegister", 0U);

		// レジスタ空間
		l_descriptorRange.RegisterSpace = l_jsonArray[l_i].value("RegisterSpace", 0U);

		// ディスクリプタテーブル先頭からのオフセット
		l_descriptorRange.OffsetInDescriptorsFromTableStart = l_jsonArray[l_i].value("OffsetInDescriptorsFromTableStart", D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	}

	// ディスクリプタテーブルで使用するディスクリプタレンジの数
	a_rootParameter.rootParameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(l_descriptorRangeList->size());

	// ディスクリプタテーブルで参照するディスクリプタレンジのポインタ
	a_rootParameter.rootParameter.DescriptorTable.pDescriptorRanges = l_descriptorRangeList->data();
}

nlohmann::json FWK::JsonConverter::RootSignatureJsonConverter::SerializeDescriptorRangeList(const Struct::RootParameter& a_rootParameter) const
{
	// ディスクリプタレンジリストが存在しなければreturn
	if (!a_rootParameter.descriptorRangeList) { return {}; }

	nlohmann::json l_rootJson  = {};
	auto		   l_jsonArray = nlohmann::json::array();

	for (const auto& l_descriptorRange : *a_rootParameter.descriptorRangeList)
	{
		nlohmann::json l_json = {};

		l_json["RangeType"]                         = l_descriptorRange.RangeType;
		l_json["NumDescriptors"]                    = l_descriptorRange.NumDescriptors;
		l_json["BaseShaderRegister"]                = l_descriptorRange.BaseShaderRegister;
		l_json["RegisterSpace"]                     = l_descriptorRange.RegisterSpace;
		l_json["OffsetInDescriptorsFromTableStart"] = l_descriptorRange.OffsetInDescriptorsFromTableStart;

		l_jsonArray.emplace_back(l_json);
	}

	l_rootJson["DescriptorRangeList"] = l_jsonArray;

	return l_rootJson;
}