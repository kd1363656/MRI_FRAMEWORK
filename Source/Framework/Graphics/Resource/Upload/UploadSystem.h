#pragma once

namespace FWK::Graphics
{
	class UploadSystem final
	{
	public:

		 UploadSystem() = default;
		~UploadSystem() = default;

		bool Create(const Device& a_device);

	private:

		CopyCommandQueue     m_copyCommandQueue     = {};
		CopyCommandAllocator m_copyCommandAllocator = {};
		CopyCommandList      m_copyCommandList      = {};
	};
}