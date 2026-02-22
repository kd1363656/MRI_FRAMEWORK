#pragma once

namespace FWK::Render
{
	class CommandAllocatorBase
	{
	public:

		explicit CommandAllocatorBase(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandAllocatorBase();

		void Init  ();
		bool Create();

		void Reset() const;

		const auto& GetCommandAllocator() const { return m_commandAllocator; }

	private:

		const Hardware				  k_hardware;
		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		TypeDEF::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	};
}