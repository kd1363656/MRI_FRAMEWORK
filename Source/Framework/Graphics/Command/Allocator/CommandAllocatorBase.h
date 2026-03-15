#pragma once

namespace FWK::Graphics
{
	class CommandAllocatorBase
	{
	public:

		explicit CommandAllocatorBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandAllocatorBase();

		void Init();
		bool Create(const Device& a_device);

		void Reset() const;

		void SetSubmittedFenceValue(const UINT64& a_set) { m_submittedFenceValue = a_set; }

		const auto& GetCommandAllocator() const { return m_commandAllocator; }

		auto GetCreateCommandListType() const { return k_createCommandListType; }

		const UINT64& GetSubmittedFenceValue() const { return m_submittedFenceValue; }

	private:

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		UINT64 m_submittedFenceValue;
	};
}