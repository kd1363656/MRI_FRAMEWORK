#pragma once

namespace FWK::Render
{
	class CommandContextBase
	{
	public:

		explicit CommandContextBase(const D3D12_COMMAND_LIST_TYPE a_createCommandListType);
		virtual ~CommandContextBase();

	protected:

		auto GetCreateCommandListType() const { return k_createCommandListType; }

	private:

		const D3D12_COMMAND_LIST_TYPE k_createCommandListType;
	};
}