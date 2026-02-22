#include "SwapChainJsonConverter.h"

FWK::Converter::SwapChainJsonConverter::SwapChainJsonConverter(Render::SwapChain& a_swapChain) :
	m_swapChain(a_swapChain)
{}
FWK::Converter::SwapChainJsonConverter::~SwapChainJsonConverter() = default;

void FWK::Converter::SwapChainJsonConverter::Deserialize(const nlohmann::json& a_rootJson)
{
	if (a_rootJson.is_null()) { return; }

	const UINT l_backBufferNum = a_rootJson.value("BackBufferNum", CommonConstant::k_defaultBackBufferNum);

	m_swapChain.SetBackBufferNum(l_backBufferNum);
}
nlohmann::json FWK::Converter::SwapChainJsonConverter::Serialize()
{
	auto l_rootJson = nlohmann::json{};

	l_rootJson["BackBufferNum"] = m_swapChain.GetBackBufferNum();

	return l_rootJson;
}