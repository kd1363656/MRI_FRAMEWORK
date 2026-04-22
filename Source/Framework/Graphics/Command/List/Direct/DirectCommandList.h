#pragma once

namespace FWK::Graphics
{
	class DirectCommandList final : public CommandListBase
	{
	public:

		 DirectCommandList();
		~DirectCommandList() override;
	};
}