#pragma once

namespace FWK::Render
{
	class CommandQueue final
	{
		CommandQueue ();
		~CommandQueue();

		void Init  ();
		bool Create();

	private:

		TypeDEF::ComPtr<ID3D12CommandQueue> m_commandQueue;
	};
}