// VS/PS共通
#include "../VertexToPixelHeader.hlsli"

//VS
#include "PixelShader3DHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit


// 定数バッファ：スロット0番目(b0と書く)
cbuffer cbParam : register(b0)
{
	float4 g_color;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	// UV座標とテクスチャを参照して、最適な色を取得する
	float4 TextureDiffuseColor;
   
	//float4 one = {1.0f,1.0f,1.0f,1.0f};
	float4 add = { 0.3f, 0.3f, 0.3f, 0.0f };
    
	float2 uv = PSInput.uv;
	TextureDiffuseColor = diffuseMapTexture.Sample(diffuseMapSampler, uv);

	
	float3 lightDir = PSInput.lightDir;

	
	float4 srcCol = g_common.material.diffuse * TextureDiffuseColor;
	
    // NormとLightのベクトルが反対方向を向いているほど色が明るくなるはず。
    // でも反対方向の内積は0に近づいていくので、逆にする必要がある。
    //↓0～1のモノを1～0にするには、-1をかけて0～-1にしたあと、1を足せばよい。
	float dotNormLight = dot(PSInput.normal, lightDir);
	dotNormLight = max(dotNormLight, -dotNormLight);
	srcCol.xyz *= (dotNormLight);
	return srcCol;
}