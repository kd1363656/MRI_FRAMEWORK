#include "Renderer.h"

FWK::Graphics::Renderer::Renderer(const Hardware& a_hardware) :
	k_hardware          (a_hardware),
	m_directCommandQueue(a_hardware.GetDevice()),
	m_directCommandList (a_hardware.GetDevice()),
	m_currentFrameIndex (0ULL)
{}
FWK::Graphics::Renderer::~Renderer() = default;

void FWK::Graphics::Renderer::Init()
{
	// 確保が終わってから初期化
	for (auto& l_frameResource : m_frameResourceList)
	{
		l_frameResource.Init();
	}

	m_directCommandQueue.Init();
	m_directCommandList.Init ();

	// フレームリソースの要素数を確保
	m_frameResourceList.reserve(k_frameCount);

	for (std::size_t l_i = 0ULL; l_i < k_frameCount; ++l_i)
	{
		m_frameResourceList.emplace_back(k_hardware.GetDevice());
	}

	m_currentFrameIndex = 0ULL;
}
bool FWK::Graphics::Renderer::Create()
{
	if (!m_directCommandQueue.Create())
	{
		assert(false && "ダイレクトコマンドキューの作成に失敗。");
		return false;
	}

	if (!m_directCommandList.Create())
	{
		assert(false && "ダイレクトコマンドリストの作成に失敗。");
		return false;
	}

	for (auto& l_frameResource : m_frameResourceList)
	{
		if (!l_frameResource.Create())
		{
			assert(false && "フレームリソースの作成に失敗しました。");
			return false;
		}
	}

	return true;
}