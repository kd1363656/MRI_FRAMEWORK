#include "SpriteStandardShader.hlsli"

float4 main(MeshOutput a_input) : SV_Target0
{
    float4 l_textureColor = g_baseColorTexture.Sample(g_baseColorSampler, a_input.uv);
    
    float4 l_outputColor = l_textureColor;

    // RGBは加算職として扱う
    l_outputColor.rgb += g_color.rgb;
    
    // Alphaは透過率として扱う
    l_outputColor.a *= g_color.a;
    
    return l_outputColor;
}