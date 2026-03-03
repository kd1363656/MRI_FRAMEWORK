#pragma once

namespace FWK::Graphics
{
	class DescriptorHeapContext final
	{
	public:

		DescriptorHeapContext ();
		~DescriptorHeapContext();

		void Init  ();
		bool Create();

	private:


	};
}