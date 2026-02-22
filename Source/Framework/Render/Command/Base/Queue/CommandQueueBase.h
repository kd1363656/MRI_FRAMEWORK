#pragma once

namespace FWK::Render
{
	class CommandQueueBase
	{
	public:

		explicit CommandQueueBase(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandQueueBase();

		void Init  ();
		bool Create();

		const auto& GetCommandQueue() const { return m_commandQueue; }

	private:

		const Hardware&               k_hardware;
		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		TypeDEF::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
	};
}