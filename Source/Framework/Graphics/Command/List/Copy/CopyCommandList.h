#pragma once

namespace FWK::Graphics
{
	class CopyCommandList final : public CommandListBase
	{
	public:

		explicit CopyCommandList(const Device& a_device);
		~CopyCommandList        ()                       override;

		void CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION& l_dest, D3D12_TEXTURE_COPY_LOCATION& l_src) const;
	};
}