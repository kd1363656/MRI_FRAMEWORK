#pragma once

namespace FWK::Graphics
{
	class CommandListBase
	{
	public:

		explicit CommandListBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandListBase();

		bool Create(const Device& a_device);

	private:

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		TypeAlias::ComPtr<ID3D12GraphicsCommandList6> m_commandList;
	};
}