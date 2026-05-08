#include "ModelBinaryConverter.h"

bool FWK::Converter::ModelBinaryConverter::SaveStaticModelAsset(const std::vector<Struct::ModelMeshletData>& a_modelMeshletDataList, const Struct::ModelData& a_modelData, const std::filesystem::path& a_filePath)
{
    if (a_filePath.extension() != Constant::k_lowerAssetExtension)
    {
        assert(false && "ModelAssetの拡張子が.assetではないため、保存に失敗しました。");
        return false;
    }

    if (a_modelData.m_modelMeshList.empty())
    {
        assert(false && "ModelDataのMeshListが空のため、ModelAssetの保存に失敗しました。");
        return false;
    }

    if (a_modelData.m_modelMeshList.size() != a_modelMeshletDataList.size())
    {
        assert(false && "ModelMesh数とModelMeshletData数が一致しないため、ModelAssetの保存に失敗しました。");
        return false;
    }

    const auto l_fileSize = CalculateStaticModelAssetFileSize(a_modelMeshletDataList, a_modelData);

    if (l_fileSize == k_emptyFileSize)
    {
        assert(false && "ModelAssetのファイルサイズが0のため、保存に失敗しました。");
        return false;
    }

    if (!CreateWriteMemoryMappedFile(a_filePath, l_fileSize))
    {
        assert(false && "ModelAsset用MemoryMappedFileの作成に失敗しました。");
        return false;
    }

    auto* l_basePTR = GetMutablePTRMappedData();

    if (!l_basePTR)
    {
        assert(false && "ModelAssetの書き込み先ポインタが無効です。");
        return false;
    }

    auto l_writeOffset = k_initialWriteOffset;

    ModelAssetHeader l_modelAssetHeader = {};

    l_modelAssetHeader.m_fileSize = l_fileSize;
    l_modelAssetHeader.m_magic    = k_modelAssetMagic;
    l_modelAssetHeader.m_version  = k_modelAssetVersion;

    // モデルのメッシュ数がstd::uint32_t型を超えるならreturn
    if (a_modelData.m_modelMeshList.size() > std::numeric_limits<std::uint32_t>::max())
    {
        assert(false && "StaticModelMesh数がuint32_tの最大値を超えているため、ModelAssetの保存に失敗しました。");
        return false;
    }

    l_modelAssetHeader.m_meshCount = static_cast<std::uint32_t>(a_modelData.m_modelMeshList.size());

    if (!WriteBinaryData(sizeof(ModelAssetHeader),
                         l_fileSize,
                         &l_modelAssetHeader,
                         l_writeOffset,
                         l_basePTR))
    {
        assert(false && "ModelAssetHeaderの書き込みに失敗しました。");
        return false;
    }

    for (std::size_t l_meshIndex = 0ULL; l_meshIndex < a_modelData.m_modelMeshList.size(); ++l_meshIndex)
    {
        const auto& l_staticModelMesh  = a_modelData.m_modelMeshList[l_meshIndex];
        const auto& l_modelMeshletData = a_modelMeshletDataList     [l_meshIndex];

        ModelAssetMeshHeader l_modelAssetMeshHeader = {};

        l_modelAssetMeshHeader.m_vertexCount            = l_staticModelMesh.m_modelVertexList.size       ();
        l_modelAssetMeshHeader.m_indexCount             = l_staticModelMesh.m_indexList.size             ();
        l_modelAssetMeshHeader.m_meshletCount           = l_modelMeshletData.m_meshletList.size          ();
        l_modelAssetMeshHeader.m_uniqueVertexIndexCount = l_modelMeshletData.m_uniqueVertexIndexList.size();
        l_modelAssetMeshHeader.m_primitiveIndexCount    = l_modelMeshletData.m_primitiveIndexList.size   ();

        if (!WriteBinaryData(sizeof(ModelAssetMeshHeader),
                             l_fileSize,
                             &l_modelAssetMeshHeader,
                             l_writeOffset,
                             l_basePTR))
        {
            assert(false && "ModelAssetMeshHeaderの書き込みに失敗しました。");
            return false;
        }
        
        if (!WriteBinaryData(l_staticModelMesh.m_modelVertexList.size() * sizeof(Struct::ModelVertex),
                             l_fileSize,
                             l_staticModelMesh.m_modelVertexList.data(),
                             l_writeOffset,
                             l_basePTR))
        {
            assert(false && "StaticModelVertexListの書き込みに失敗しました。");
            return false;
        }

        if (!WriteBinaryData(l_staticModelMesh.m_indexList.size() * sizeof(std::uint32_t),
                             l_fileSize,
                             l_staticModelMesh.m_indexList.data(),
                             l_writeOffset,
                             l_basePTR))
        {
            assert(false && "IndexListの書き込みに失敗しました。");
            return false;
        }

        if (!WriteBinaryData(l_modelMeshletData.m_meshletList.size() * sizeof(Struct::ModelMeshlet),
                             l_fileSize,
                             l_modelMeshletData.m_meshletList.data(),
                             l_writeOffset,
                             l_basePTR))
        {
            assert(false && "ModelMeshletListの書き込みに失敗しました。");
            return false;
        }

        if (!WriteBinaryData(l_modelMeshletData.m_uniqueVertexIndexList.size() * sizeof(std::uint32_t),
                             l_fileSize,
                             l_modelMeshletData.m_uniqueVertexIndexList.data(),
                             l_writeOffset,
                             l_basePTR))
        {
            assert(false && "UniqueVertexIndexListの書き込みに失敗しました。");
            return false;
        }

        if (!WriteBinaryData(l_modelMeshletData.m_primitiveIndexList.size() * sizeof(std::uint8_t),
                             l_fileSize,
                             l_modelMeshletData.m_primitiveIndexList.data(),
                             l_writeOffset,
                             l_basePTR))
        {
            assert(false && "PrimitiveIndexListの書き込みに失敗しました。");
            return false;
        }
    }

    if (l_writeOffset != l_fileSize)
    {
        assert(false && "ModelAssetの計算ファイルサイズと実際の書き込みサイズが一致しません。");
        return false;
    }

    return true;
}

std::uint64_t FWK::Converter::ModelBinaryConverter::CalculateStaticModelAssetFileSize(const std::vector<Struct::ModelMeshletData>& a_modelMeshletDataList, const Struct::ModelData& a_modelData) const
{
    std::uint64_t l_fileSize = sizeof(ModelAssetHeader);

    for (std::size_t l_meshIndex = 0U; l_meshIndex < a_modelData.m_modelMeshList.size(); ++l_meshIndex)
    {
        const auto& l_modelMesh        = a_modelData.m_modelMeshList[l_meshIndex];
        const auto& l_modelMeshletData = a_modelMeshletDataList     [l_meshIndex];

        l_fileSize += sizeof(ModelAssetMeshHeader);

        l_fileSize += l_modelMesh.m_modelVertexList.size() * sizeof(Struct::ModelVertex);
        l_fileSize += l_modelMesh.m_indexList.size      () * sizeof(std::uint32_t);

        l_fileSize += l_modelMeshletData.m_meshletList.size          () * sizeof(Struct::ModelMeshlet);
        l_fileSize += l_modelMeshletData.m_uniqueVertexIndexList.size() * sizeof(std::uint32_t);
        l_fileSize += l_modelMeshletData.m_primitiveIndexList.size   () * sizeof(std::uint8_t);
    }
    
    return l_fileSize;
}

bool FWK::Converter::ModelBinaryConverter::WriteBinaryData(const std::uint64_t& a_writeSize,
                                                           const std::uint64_t& a_fileSize,
                                                           const void*          a_sourcePTR,
                                                                 std::uint64_t& a_writeOffset,
                                                                 std::uint8_t*  a_basePTR) const
{
    if (a_writeSize == k_emptyWriteSize) { return true; }

    if (!a_basePTR)
    {
        assert(false && "BinaryDataの書き込み先ポインタが無効です。");
        return false;
    }

    if (!a_sourcePTR)
    {
        assert(false && "BinaryDataの書き込み元ポインタが無効です。");
        return false;
    }
    
    if (a_writeOffset > a_fileSize)
    {
        assert(false && "BinaryDataの書き込み開始位置がファイルサイズを超えています。");
        return false;
    }
 
    if (a_writeSize > a_fileSize - a_writeOffset)
    {
        assert(false && "BinaryDataの書き込み範囲がファイルサイズを超えています。");
        return false;
    }
    
    // オフセット分ポインタをずらしてからコピー
    std::memcpy(a_basePTR + a_writeOffset, a_sourcePTR, a_writeSize);
    
    a_writeOffset += a_writeSize;

    return true;
}