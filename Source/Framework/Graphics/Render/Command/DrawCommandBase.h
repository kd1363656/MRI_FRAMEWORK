#pragma once

namespace FWK::Graphics
{
	template <typename Type>
	class DrawCommandBase : public IDrawCommand
	{
	private:

		struct GraphicsPipelineSetupResult final
		{
			const RootSignature* m_rootSignature = nullptr;
			const PipelineState* m_pipelineState = nullptr;
		};

	public:

		 DrawCommandBase()		    = default;
		~DrawCommandBase() override = default;

		void BeginFrame() override
		{
			m_drawCommandList.clear();
		}

		void RequestDraw(const Type& a_drawCommand)
		{
			m_drawCommandList.emplace_back(a_drawCommand);
		}

		const auto& GetREFDrawCommandList() const { return m_drawCommandList; }

	protected:

		template <Concept::IsDerivedPipelineStateTagBaseConcept PipelineStateType>
		GraphicsPipelineSetupResult SetupGraphicsPipelineStateByTag(const Renderer& a_renderer) const
		{
			auto* l_pipelineState = a_renderer.FindPTRPipelineState(Utility::Tag::GetTag<PipelineStateType>());

			if (!l_pipelineState) 
			{
				assert(false && "使用するパイプラインステートが作成されておらず、描画を開始できませんでした。");
				return {};
			}

			// パイプラインステートが使用するルートシグネチャを取得
			auto* l_rootSignature = a_renderer.FindPTRRootSignature(l_pipelineState->GetVALUseRootSignatureTag());

			if (!l_rootSignature)
			{
				assert(false && "使用するルートシグネチャが作成されておらず、描画を開始できませんでした。");
				return {};
			}

			const auto& l_directCommandList = a_renderer.GetREFDirectCommandList();

			// ルートシグネチャをセット
			l_directCommandList.SetupRootSignature(l_rootSignature);

			// パイプラインステートをセット
			l_directCommandList.SetupPipelineState(l_pipelineState);

			// 使用したルートシグネチャ、パイプラインステートを外部に渡す
			return { l_rootSignature, l_pipelineState };
		}

	private:

		std::vector<Type> m_drawCommandList = {};
	};
}