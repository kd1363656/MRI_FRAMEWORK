#pragma once

namespace FWK::Editor
{
	class EditorManager final : public SingletonBase<EditorManager>
	{
	public:

		void Init(const Graphics::Device&									   a_device,
				  const Graphics::DirectCommandQueue&						   a_directCommandQueue,
				  const HWND&												   a_hwnd,
				  const std::size_t											   a_numFramesInFlight,
						Graphics::DescriptorPool<Graphics::SRVDescriptorHeap>& a_srvDescriptorHeap);

		void DrawEditor(Graphics::Renderer& a_renderer, const Graphics::DescriptorPool<Graphics::SRVDescriptorHeap>& a_srvDescriptorHeap);

	private:

		void Release() const;

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<EditorManager>;

		 EditorManager();
		~EditorManager() override;
	};
}