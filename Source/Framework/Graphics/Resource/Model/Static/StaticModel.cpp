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
{
	a_other.m_storageID = Constant::k_invalidStorageID;
	a_other.m_staticModelRecord.reset();
}
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

	return *this;
}
FWK::Graphics::StaticModel& FWK::Graphics::StaticModel::operator=(StaticModel&& a_other) noexcept
{
	if (this == &a_other) { return *this; }

	// 所持しているStaticModelを破棄
	ReleaseStaticModelReference();

	// ムーブでは参照数を増やさず、参照先だけ移す
	m_storageID			= a_other.m_storageID;
	m_staticModelRecord = std::move(a_other.m_staticModelRecord);

	// 参照元のStorageIDを無効化
	a_other.m_storageID = Constant::k_invalidStorageID;
	a_other.m_staticModelRecord.reset();

	return *this;
}

bool FWK::Graphics::StaticModel::Load(const std::filesystem::path& a_filePath)
{
	// 既に別のStorageIDを持っている場合は先に参照を外す
	ReleaseStaticModelReference();

		  auto& l_graphicsManager    = GraphicsManager::GetInstance                    ();
		  auto& l_resourceContext    = l_graphicsManager.GetMutableREFResourceContext  ();
		  auto& l_staticModelSystem  = l_resourceContext.GetMutableREFStaticModelSystem();
	const auto& l_device		     = l_graphicsManager.GetREFDevice		           ();
	const auto& l_gpuMemoryAllocator = l_resourceContext.GetREFGPUMemoryAllocator      ();
		  auto& l_srvDescriptorPool  = l_resourceContext.GetMutableREFSRVDescriptorPool();

	const auto& l_staticModelLoadResult = l_staticModelSystem.LoadStaticModelForBatchUpload(l_device, 
																						    l_gpuMemoryAllocator,
																							a_filePath,
																							l_srvDescriptorPool);


	if (l_staticModelLoadResult.m_storageID == Constant::k_invalidStorageID)
	{
		assert(false && "StaticModelの読み込みに失敗しました。");
		return false;
	}

	if (l_staticModelLoadResult.m_staticModelRecord.expired())
	{
		assert(false && "StaticModelRecordが無効のため、StaticModelの読み込みに失敗しました。");
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

	if (!l_staticModelSystem.AddStaticModelReference(m_staticModelRecord)) 
	{
		assert(false && "StaticModelの参照数加算に失敗しました。");
		return;
	}
}

void FWK::Graphics::StaticModel::ReleaseStaticModelReference()
{
	if (m_storageID == Constant::k_invalidStorageID)
	{
		m_staticModelRecord.reset();
		return; 
	}

	      auto& l_graphicsManager    = GraphicsManager::GetInstance                    ();
	      auto& l_resourceContext    = l_graphicsManager.GetMutableREFResourceContext  ();
	const auto& l_renderer		   = l_graphicsManager.GetREFRenderer				   ();
	const auto& l_directCommandQueue = l_renderer.GetREFDirectCommandQueue             ();

	if (auto& l_staticModelSystem = l_resourceContext.GetMutableREFStaticModelSystem(); 
		!l_staticModelSystem.ReleaseStaticModelReference(l_directCommandQueue, m_staticModelRecord))
	{
		assert(false && "StaticModelの参照数減算に失敗しました。");
		return; 
	}

	m_storageID = Constant::k_invalidStorageID;
	m_staticModelRecord.reset();
}