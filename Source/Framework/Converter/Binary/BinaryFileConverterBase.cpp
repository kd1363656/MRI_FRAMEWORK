#include "BinaryFileConverterBase.h"

bool FWK::Converter::BinaryFileConverterBase::LoadBinaryFile(std::vector<std::uint8_t>& a_binaryDataList, const std::filesystem::path& a_filePath) const
{
	return false;
}

bool FWK::Converter::BinaryFileConverterBase::SaveBinaryFile(const std::vector<std::uint8_t>& a_binaryDataList, const std::filesystem::path& a_filePath) const
{
	return false;
}