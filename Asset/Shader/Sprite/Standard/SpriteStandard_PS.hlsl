// SpriteStandard の最小動作確認用PixelShader
// いまはテクスチャサンプリング前段階なので、入力色をそのまま返す

struct PixelInput
{
	float4 position : SV_Position;
	float4 color    : COLOR;
};

float4 main(PixelInput a_input) : SV_Target0
{
	return a_input.color;
}