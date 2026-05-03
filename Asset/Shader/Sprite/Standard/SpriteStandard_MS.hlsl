// SpriteStandard の最小動作確認用MeshShader
// いまはスプライト描画前段階なので、四角形を直書きしてテクスチャ表示確認を行う
// AmplificationShaderはまだ使っていないため、payload入力は持たせない

struct MeshOutput
{
	float4 position : SV_Position;
	float2 uv       : TEXCOORD0;
};

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(out vertices MeshOutput a_vertexList[4],
		  out indices  uint3      a_primitiveList[2])
{
	// SetMeshOutputCounts(出力頂点数、出力プリミティブ数);
	SetMeshOutputCounts(4, 2);

	a_vertexList[0].position = float4(-0.5f,  0.5f, 0.0f, 1.0f);
	a_vertexList[1].position = float4( 0.5f,  0.5f, 0.0f, 1.0f);
	a_vertexList[2].position = float4(-0.5f, -0.5f, 0.0f, 1.0f);
	a_vertexList[3].position = float4( 0.5f, -0.5f, 0.0f, 1.0f);

	a_vertexList[0].uv = float2(0.0f, 0.0f);
	a_vertexList[1].uv = float2(1.0f, 0.0f);
	a_vertexList[2].uv = float2(0.0f, 1.0f);
	a_vertexList[3].uv = float2(1.0f, 1.0f);

	a_primitiveList[0] = uint3(0, 1, 2);
	a_primitiveList[1] = uint3(2, 1, 3);
}