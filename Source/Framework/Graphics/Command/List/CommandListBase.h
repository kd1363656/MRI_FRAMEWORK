#pragma once

namespace FWK::Graphics
{
	class CommandListBase
	{
	public:

		explicit CommandListBase (const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual  ~CommandListBase();

		void Init  ();
		bool Create();

		void Reset(const CommandAllocatorBase& a_commandAllocator) const;

		void TransitionResource(const ComPtr<ID3D12Resource2>& a_resourece, const D3D12_RESOURCE_STATES a_beforState, const D3D12_RESOURCE_STATES a_afterState) const;

		const auto& GetCommandList() const { return m_commandList; }

		auto GetCreateCommandListType() const { return k_createCommandListType; }

	private:

		static constexpr UINT k_sendBarrierNum = 1U;

		const Device& k_device;

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		ComPtr<ID3D12GraphicsCommandList6> m_commandList;
	};
}