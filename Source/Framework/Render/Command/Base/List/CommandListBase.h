#pragma once

namespace FWK::Render
{
	class CommandListBase
	{
	public:

		explicit CommandListBase(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandListBase();

		void Init  ();
		bool Create();

	private:

		const Hardware&				  k_hardware;
		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;

		TypeDEF::ComPtr<ID3D12GraphicsCommandList6> m_commandList = nullptr;
	};
}