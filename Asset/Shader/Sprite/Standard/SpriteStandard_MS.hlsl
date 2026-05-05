#include "SpriteStandardShader.hlsli"

[outputtopology("triangle")]

[numthreads(1, 1, 1)]


void main(out vertices MeshOutput a_vertexList   [4],
		  out indices  uint3      a_primitiveList[2])
{
	// SetMeshOutputCounts(出力頂点数、
	//					   出力プリミティブ数);
	
	SetMeshOutputCounts(4, 2);

    uint l_textureWidth  = 0U;
    uint l_textureHeight = 0U;
	
	// GetDimensions(テクスチャの横幅、
	//				 テクスチャの縦幅);
	
    g_baseColorTexture.GetDimensions(l_textureWidth, l_textureHeight);
	
    const float2 l_textureSize = float2((float)l_textureWidth, (float)l_textureHeight);

	// sourceRECTのwidth / heightが0より大きければ部分描画として扱う
	// 大きくなければCPU空の転送に失敗しているため使用しない
    const bool l_hasSourceRECT = (g_sourceRECT.z > 0U) && (g_sourceRECT.w > 0U);

	// sourceRECT指定あり : sourceRECTのサイズ
	// sourceRECT指定なし : テクスチャ全体サイズ
    const float2 l_baseSize   = l_hasSourceRECT ? float2((float) g_sourceRECT.z, (float) g_sourceRECT.w) : l_textureSize;
    const float2 l_spriteSize = l_baseSize * g_scale;
	
	// pivotを基準にローカル座標を作成する
	// pivot = float2(0.0F, 0.0F) : 左上基準
	// pivot = float2(0.5F, 0.5F) : 中心基準
	// pivot = float2(1.0F, 1.0F) : 右下基準
    const float l_left   = -l_spriteSize.x * g_pivot.x;
    const float l_right  =  l_left         + l_spriteSize.x;
    const float l_top    =  l_spriteSize.y * g_pivot.y;
    const float l_bottom =  l_top          - l_spriteSize.y;
	
    const float2 l_localPositionList[4] =
    {
        float2(l_left,  l_bottom),
		float2(l_left,  l_top),
		float2(l_right, l_bottom),
		float2(l_right, l_top)
    };

	// UV座標を計算(g_sourceRECTからテクスチャのサイズを割るとUV座標が分かる)
    const float2 l_uvMIN = l_hasSourceRECT ? float2((float)g_sourceRECT.x,					(float)g_sourceRECT.y)					/ l_textureSize : float2(0.0F, 0.0F);
    const float2 l_uvMAX = l_hasSourceRECT ? float2((float)g_sourceRECT.x + g_sourceRECT.z, (float)g_sourceRECT.y + g_sourceRECT.w) / l_textureSize : float2(1.0F, 1.0F);
	
	[unroll]
    for (uint l_i = 0U; l_i < 4U; ++l_i)
    {
        const float2 l_worldPosition = l_localPositionList[l_i] + g_position;
		
		// 画面座標を正射影行列でクリップ座標へ変換する
        a_vertexList[l_i].position = mul(float4(l_worldPosition, 0.0F, 1.0F), g_projectionMatrix);
    }
	
    a_vertexList[0].uv = float2(l_uvMIN.x, l_uvMAX.y);
    a_vertexList[1].uv = float2(l_uvMIN.x, l_uvMIN.y);
    a_vertexList[2].uv = float2(l_uvMAX.x, l_uvMAX.y);
    a_vertexList[3].uv = float2(l_uvMAX.x, l_uvMIN.y);

    a_primitiveList[0] = uint3(0U, 1U, 2U);
    a_primitiveList[1] = uint3(2U, 1U, 3U);
}