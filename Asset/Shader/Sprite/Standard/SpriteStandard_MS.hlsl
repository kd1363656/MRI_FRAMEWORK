// SpriteStandard の最小動作確認用MeshShader
// いまはテクスチャ描画前段階なので、三角形を直書きして表示確認だけを行う
// AmplificationShaderはまだ使っていないため、payload入力は持たせない

struct MeshOutput
{
	float4 position : SV_Position;
	float4 color    : COLOR;
};

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(out vertices MeshOutput a_vertexList[3],
		  out indices uint3 a_primitiveList[1])
{
	// SetMeshOutputCounts(出力頂点数、出力プリミティブ数);
	SetMeshOutputCounts(3, 1);

	a_vertexList[0].position = float4( 0.0f,  0.5f, 0.0f, 1.0f);
	a_vertexList[1].position = float4( 0.5f, -0.5f, 0.0f, 1.0f);
	a_vertexList[2].position = float4(-0.5f, -0.5f, 0.0f, 1.0f);

	a_vertexList[0].color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	a_vertexList[1].color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	a_vertexList[2].color = float4(0.0f, 0.0f, 1.0f, 1.0f);

	a_primitiveList[0] = uint3(0, 1, 2);
}