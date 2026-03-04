#pragma once

namespace FWK::Graphics
{
	class CopyCommandList final : public CommandListBase
	{
	public:

		explicit CopyCommandList(const Device& a_device);
		~CopyCommandList        ()                       override;
	};
}