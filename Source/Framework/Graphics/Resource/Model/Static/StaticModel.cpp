#include "StaticModel.h"

FWK::Graphics::StaticModel::StaticModel() : 
	m_storageID        (Constant::k_invalidStorageID),
	m_staticModelRecord({})
{}
FWK::Graphics::StaticModel::StaticModel(const StaticModel & a_other) : 
	m_storageID		   (a_other.m_storageID),
	m_staticModelRecord(a_other.m_staticModelRecord)
{
	AddStaticModelReference();
}
FWK::Graphics::StaticModel::StaticModel(StaticModel&& a_other) noexcept :
	m_storageID		   (a_other.m_storageID),
	m_staticModelRecord(std::move(a_other.m_staticModelRecord))
{}
FWK::Graphics::StaticModel::~StaticModel()
{
	ReleaseStaticModelReference();
}

FWK::Graphics::StaticModel& FWK::Graphics::StaticModel::operator=(const StaticModel& a_other)
{
	if (this == &a_other) { return *this; }

	// 所持しているStaticModelを破棄
	ReleaseStaticModelReference();

	// コピー元と同じStaticModelを参照
	m_storageID			= a_other.m_storageID;
	m_staticModelRecord = a_other.m_staticModelRecord;

	// 参照数の加算
	AddStaticModelReference();
}
FWK::Graphics::StaticModel& FWK::Graphics::StaticModel::operator=(StaticModel&& a_other) noexcept
{
	if (this == &a_other) { return *this; }

	// 所持しているStaticModelを破棄
	ReleaseStaticModelReference();

	// ムーブでは参照数を増やさず、参照先だけ移す
	m_storageID = Constant::k_invalidStorageID;
	m_staticModelRecord.reset();

	a_other.m_storageID = Constant::k_invalidStorageID;
	a_other.m_staticModelRecord.reset();

	return *this;
}

bool FWK::Graphics::StaticModel::Load(const std::filesystem::path& a_filePath)
{
	ReleaseStaticModelReference();

	auto& l_graphicsManager   = GraphicsManager::GetInstance                    ();
	auto& l_resourceContext   = l_graphicsManager.GetMutableREFResourceContext  ();
	auto& l_staticModelSystem = l_resourceContext.GetMutableREFStaticModelSystem();

	const auto& l_staticModelLoadResult = l_staticModelSystem.LoadStaticModelForBatchUpload(a_filePath);

	if (l_staticModelLoadResult.m_storageID == Constant::k_invalidStorageID)
	{
		assert(false && "StaticModelの読み込みに失敗しました。");
		return false;
	}

	m_storageID         = l_staticModelLoadResult.m_storageID;
	m_staticModelRecord = l_staticModelLoadResult.m_staticModelRecord;

	return true;
}

bool FWK::Graphics::StaticModel::IsValid() const
{
	if (m_storageID == Constant::k_invalidStorageID) { return false; }
	if (m_staticModelRecord.expired())				 { return false; }

	return true;
}

void FWK::Graphics::StaticModel::AddStaticModelReference() const
{
	if (m_storageID == Constant::k_invalidStorageID) { return; }

	auto& l_graphicsManager   = GraphicsManager::GetInstance			        ();
	auto& l_resourceContext   = l_graphicsManager.GetMutableREFResourceContext  ();
	auto& l_staticModelSystem = l_resourceContext.GetMutableREFStaticModelSystem();

	if (!l_staticModelSystem.AddStaticModelReference(m_storageID)) 
	{
		assert(false && "StaticModelの参照数加算に失敗しました。");
		return;
	}
}

void FWK::Graphics::StaticModel::ReleaseStaticModelReference()
{
	if (m_storageID == Constant::k_invalidStorageID) { return; }

	      auto& l_graphicsManager    = GraphicsManager::GetInstance                    ();
	      auto& l_resourceContext    = l_graphicsManager.GetMutableREFResourceContext  ();
	      auto& l_staticModelSystem  = l_resourceContext.GetMutableREFStaticModelSystem();
	const auto& l_renderer		   = l_graphicsManager.GetREFRenderer				   ();
	const auto& l_directCommandQueue = l_renderer.GetREFDirectCommandQueue             ();

	if (l_staticModelSystem.ReleaseStaticModelReference(l_directCommandQueue, m_storageID)) { return; }

	assert(false && "StaticModelの参照数減算に失敗しました。");
}