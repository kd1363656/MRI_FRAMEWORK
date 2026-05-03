// SpriteStandard の最小動作確認用PixelShader
// いまはテクスチャサンプリング前段階なので、入力色をそのまま返す

Texture2D    g_baseColorTexture : register(t0);
SamplerState g_baseColorSampler : register(s0);

struct PixelInput
{
	float4 position : SV_Position;
	float2 uv		: TEXCOORD0;
};

float4 main(PixelInput a_input) : SV_Target0
{
    return g_baseColorTexture.Sample(g_baseColorSampler, a_input.uv);
}