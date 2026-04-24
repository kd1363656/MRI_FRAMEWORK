#include "TextureLoader.h"

bool FWK::Graphics::TextureLoader::LoadTextureFile(DirectX::ScratchImage& a_scratchImage, DirectX::TexMetadata& a_texMetadata, const std::filesystem::path& a_sourcePath) const
{
	if (a_sourcePath.empty())
	{
		assert(false && "読み込み対象のDDSテクスチャファイルパスが空のため、DDS読み込みに失敗しました。");
		return false;
	}

	const auto l_extension = a_sourcePath.extension();

	if (l_extension != Constant::k_lowerDDSExtension)
	{
		assert(false && "DDS以外の拡張子が指定されたため、DDS読み込みに失敗しました。");
		return false;
	}

	a_scratchImage = {};
	a_texMetadata  = {};

	const auto l_windowSourcePath = a_sourcePath.wstring();

	// LoadFromDDSFileについて
	// LoadFromDDSFile(読み込みDDSファイルパス	、
	//				   DDS読み込み時の追加フラグ、
	//				   DDSの幅、高さ、mip数、DXGI_FORMATなどを受け取る構造体、
	//				   実際の画像ピクセルデータを受け取るScratchImage);
	const auto l_hr = DirectX::LoadFromDDSFile(l_windowSourcePath.c_str(),
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

bool FWK::Graphics::TextureLoader::IsSRGBFormat(const DXGI_FORMAT a_format) const
{
	switch (a_format)
	{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return true;
		break;

		default:
			return false;
		break;
	}
}
bool FWK::Graphics::TextureLoader::IsUNORMFormat(const DXGI_FORMAT a_format) const
{
	switch (a_format)
	{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_BC7_UNORM:
			return true;
		break;

		default:
			return false;
		break;
	}
}