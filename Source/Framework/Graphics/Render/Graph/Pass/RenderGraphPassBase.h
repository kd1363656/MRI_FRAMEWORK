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

		void ReadTexture(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_usageTag);
		
		void WriteTexture(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_usageTag);

	private:

		void AddTextureAccess(const TypeAlias::TypeTag a_textureTag, const TypeAlias::TypeTag a_accessTag, const TypeAlias::TypeTag a_usageTag);

		std::vector<Struct::RenderGraphTextureAccess> m_textureAccessList  = {};

		FWK_DEFINE_TYPE_INFO(RenderGraphPassBase, IRenderGraphPass)
	};
}