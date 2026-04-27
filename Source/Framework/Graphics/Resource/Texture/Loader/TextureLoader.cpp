#include "TextureLoader.h"

bool FWK::Graphics::TextureLoader::LoadTextureFile(DirectX::ScratchImage& a_scratchImage, DirectX::TexMetadata& a_texMetadata, const std::wstring& a_filePath) const
{
	if (a_filePath.empty())
	{
		assert(false && "読み込み対象のDDSテクスチャファイルパスが空のため、DDS読み込みに失敗しました。");
		return false;
	}

	a_scratchImage = {};
	a_texMetadata  = {};

	// LoadFromDDSFileについて
	// LoadFromDDSFile(読み込みDDSファイルパス	、
	//				   DDS読み込み時の追加フラグ、
	//				   DDSの幅、高さ、mip数、DXGI_FORMATなどを受け取る構造体、
	//				   実際の画像ピクセルデータを受け取るScratchImage);
	const auto l_hr = DirectX::LoadFromDDSFile(a_filePath.c_str(),
											   DirectX::DDS_FLAGS_NONE,
											   &a_texMetadata,
											   a_scratchImage);

	if (FAILED(l_hr))
	{
		assert(false && "DirectX::LoadFromDDSFileに失敗したため、DDS読み込みに失敗しました。");
		return false;
	}

	if (a_texMetadata.format == DXGI_FORMAT_UNKNOWN)
	{
		assert(false && "DDSのDXGI_FORMATが不明なため、DDS読み込みに失敗しました。");
		return false;
	}

	return true;
}