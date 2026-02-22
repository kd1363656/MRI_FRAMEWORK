#pragma once

namespace FWK::Render
{
	class CommandAllocatorBase
	{
	public:

		explicit CommandAllocatorBase() = default;;
		virtual ~CommandAllocatorBase() = default;;

		void Init  ();
		bool Create(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);

		void Reset() const;

		const auto& GetCommandAllocator() const { return m_commandAllocator; }

	private:

		TypeDEF::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	};
}