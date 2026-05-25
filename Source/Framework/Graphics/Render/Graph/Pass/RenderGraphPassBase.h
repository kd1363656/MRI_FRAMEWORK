#pragma once

namespace FWK::Graphics
{
	class RenderGraphPassBase : public IRenderGraphPass
	{
	public:

		 RenderGraphPassBase()			= default;
		~RenderGraphPassBase() override = default;

		const std::vector<Struct::RenderGraphTextureAccess>& GetREFTextureAccessList () const override { return m_textureAccessList; }
		
	protected:

		void ReadTexture(const D3D12_RESOURCE_STATES a_requiredState, const TypeAlias::TypeTag a_textureTag);
		
		void WriteTexture(const D3D12_RESOURCE_STATES a_requiredState, const TypeAlias::TypeTag a_textureTag);

	private:

		void AddTextureAccess(const D3D12_RESOURCE_STATES a_requiredState, const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_accessTag);

		std::vector<Struct::RenderGraphTextureAccess> m_textureAccessList  = {};
	};
}