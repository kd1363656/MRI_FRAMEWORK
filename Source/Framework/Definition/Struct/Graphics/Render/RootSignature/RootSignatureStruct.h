#pragma once

namespace FWK::Struct
{
	struct RootParameter final
	{
		// 使わない可能性を考慮してディスクリプタテーブル用のD3D12_DESCRIPTOR_RANGEをまとめたリストはポインタとして保持しておく
		// コピーコンストラクタ禁止を防ぐためにshared_ptrを使用
		std::shared_ptr<std::vector<D3D12_DESCRIPTOR_RANGE>> descriptorRangeList = nullptr;

		D3D12_ROOT_PARAMETER rootParameter = D3D12_ROOT_PARAMETER();

		TypeAlias::TypeTag rootParameterTag = Constant::k_invalidTypeTag;
	};
}