#pragma once

namespace FWK::Render
{
	class CommandQueueBase
	{
	public:

		CommandQueueBase		 () = default;
		virtual ~CommandQueueBase() = default;

		void Init  ();
		bool Create(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);

		const auto& GetCommandQueue() const { return m_commandQueue; }

	private:

		TypeDEF::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
	};
}