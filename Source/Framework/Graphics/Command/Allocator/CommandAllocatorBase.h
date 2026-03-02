#pragma once

namespace FWK::Graphics
{
	class CommandAllocatorBase
	{
	public:

		explicit CommandAllocatorBase (const Device& a_device, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual  ~CommandAllocatorBase();

		void Init  ();
		bool Create();

		void Reset() const;

	private:

		const Device& k_device;

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	};
}