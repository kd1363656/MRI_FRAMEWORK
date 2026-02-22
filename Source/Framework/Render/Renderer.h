#pragma once

// アプリケーションにクラスのインスタンスを置いておきたいが、フレームワークという都合上
// アプリケーションとの依存を極力減らしたいためシングルトンで定義
namespace FWK::Render
{
	class Renderer final : public SingletonBase<Renderer>
	{
	public:

		void Init          ();
		bool Create        (const HWND& a_hWND, const CommonStruct::WindowConfig& a_windowConfig);
		void PostCreateInit(const HWND& a_hWND);

		void LoadCONFIG();
		void SaveCONFIG();

		UINT GetSyncInterval() const { return m_syncInterval; }

		void SetSyncInterval(const UINT a_set) { m_syncInterval = a_set; }

	private:

		static constexpr D3D12_COMMAND_LIST_TYPE k_createCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;

		const std::string k_configFileIOPath = "Asset/Data/CONFIG/Renderer/RendererCONFIG.json";

		Hardware m_hardware;
		
		Converter::RendererJsonConverter m_rendererJsonConverter;

		UINT m_syncInterval;

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<Renderer>;

		Renderer ();
		~Renderer() override;
	};
}