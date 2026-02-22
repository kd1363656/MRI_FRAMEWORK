#pragma once

namespace FWK::Render
{
	class CommandListBase
	{
	public:

		CommandListBase         () = default;
		virtual ~CommandListBase() = default;

		void Init  ();
		bool Create(const Hardware& a_hardware, const D3D12_COMMAND_LIST_TYPE a_createCommandListType);

	private:

		TypeDEF::ComPtr<ID3D12GraphicsCommandList6> m_commandList = nullptr;
	};
}