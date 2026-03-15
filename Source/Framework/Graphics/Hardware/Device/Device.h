#pragma once

namespace FWK::Graphics
{
	class Device final
	{
	public:

		Device()  = default;
		~Device() = default;

		void Init();
		bool Create(const Factory& a_factory);

		UINT GetDefaultGPUNodeMask() const { return k_defaultGPUNodeMask; }

		const auto& GetDevice() const { return m_device; }

	private:

		// このDeviceクラスのID3D12Deviceを使って作るコマンドキューなどの各種GPUオブジェクトで
		// 共通使用する既定のGPUノード指定値
		// GPUノードマスクは「どのGPUノードを対象にするか」を表す値、
		// このプロジェクトでは単一GPU前提で作成するため固定値にしている
		static constexpr UINT k_defaultGPUNodeMask = 0U;

		Microsoft::WRL::ComPtr<ID3D12Device9> m_device = nullptr;
	};
}