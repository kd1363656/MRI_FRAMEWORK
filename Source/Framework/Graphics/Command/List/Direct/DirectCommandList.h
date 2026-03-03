#pragma once

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		explicit DirectCommandList(const Device& a_device);
		~DirectCommandList        ()					   override;
	};
}