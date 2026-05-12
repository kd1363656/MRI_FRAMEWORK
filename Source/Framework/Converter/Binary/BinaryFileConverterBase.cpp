#include "BinaryFileConverterBase.h"

FWK::Converter::BinaryFileConverterBase::BinaryFileConverterBase() : 
	m_mappedData(nullptr),

	m_fileHandle       (INVALID_HANDLE_VALUE),
	m_fileMappingHandle(nullptr),

	m_mappedDataSize(k_emptyMappedDataSize)
{}
FWK::Converter::BinaryFileConverterBase::~BinaryFileConverterBase()
{
	DestroyMemoryMappedFile();
}