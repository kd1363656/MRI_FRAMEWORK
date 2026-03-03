#pragma once

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		DirectCommandList (const Device& a_device);
		~DirectCommandList();
	};
}