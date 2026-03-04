struct VSOUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

// "1"スレッドで実行(最初はこれが一番安全)
[numthreads(1, 1, 1)]

// 三角形トポロジを出力
[outputtopology("triangle")]

void main(out vertices VSOUT o_vertices[4],
          out indices  uint3 o_triangles[2])
{
    // 出力数宣言(必須)
    SetMeshOutputCounts(4, 2);
    
    // 三角形生成("NDC")
    o_vertices[0].pos = float4(-0.5F, -0.5F, 0.0F, 1.0F);
    o_vertices[1].pos = float4(-0.5F,  0.5F, 0.0F, 1.0F);
    o_vertices[2].pos = float4( 0.5F, -0.5F, 0.0F, 1.0F);
    o_vertices[3].pos = float4( 0.5F,  0.5F, 0.0F, 1.0F);

    // 仮UV(まだ使わない)
    o_vertices[0].uv = float2(0, 1);
    o_vertices[1].uv = float2(0, 0);
    o_vertices[2].uv = float2(1, 1);
    o_vertices[3].uv = float2(1, 0);
    
    // インデックス
    o_triangles[0] = uint3(0, 1, 2);
    o_triangles[1] = uint3(2, 1, 3);
}