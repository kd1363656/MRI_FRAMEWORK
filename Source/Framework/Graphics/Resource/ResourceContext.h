#pragma once

namespace FWK::Graphics
{
	class SwapChain;
}

namespace FWK::Graphics
{
	class ResourceContext final
	{
	public:

		ResourceContext (const Device& a_device);
		~ResourceContext();

		void Init  ();
		bool Create(const SwapChain& a_swapChain);
		
		const auto& GetDescriptorHeapContext() const { return m_descriptorHeapContext; }

		const auto& GetCopyCommandList () const { return m_copyCommandList; }
		const auto& GetCopyCommandQueue() const { return m_copyCommandQueue; }

		auto& GetWorkSRVDescriptorAllocator() { return m_srvDescriptorAllocator; }

		auto& GetWorkCopyCommandQueue    () { return m_copyCommandQueue; }
		auto& GetWorkCopyCommandAllocator() { return m_copyCommandAllocator; }

	private:

		DescriptorHeapContext m_descriptorHeapContext;

		CopyCommandQueue     m_copyCommandQueue;
		CopyCommandAllocator m_copyCommandAllocator;
		CopyCommandList      m_copyCommandList;

		SRVDescriptorAllocator m_srvDescriptorAllocator;
	};
}