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

		void Reset(const CommandAllocatorBase* a_commandAllocator) const;

		const auto& GetCommandList() { return m_commandList; }

	private:

		const Device& k_device;

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		ComPtr<ID3D12GraphicsCommandList6> m_commandList;
	};
}