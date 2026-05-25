#pragma once

namespace FWK::Graphics
{
	class RenderGraphPassBase : public IRenderGraphPass
	{
	public:

		 RenderGraphPassBase()			= default;
		~RenderGraphPassBase() override = default;

		const std::vector<Struct::RenderGraphTextureAccess>& GetREFTextureAccessList () const override { return m_readTextureAccessList; }
		
	protected:

		void ReadTexture(const TypeAlias::TypeTag a_textureTag, const D3D12_RESOURCE_STATES a_requiredState);
		
		void WriteTexture(const TypeAlias::TypeTag a_textureTag, const D3D12_RESOURCE_STATES a_requiredState);

	private:

		std::vector<Struct::RenderGraphTextureAccess> m_readTextureAccessList  = {};
		std::vector<Struct::RenderGraphTextureAccess> m_writeTextureAccessList = {};
	};
}