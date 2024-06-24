// VS/PS����
#include "../VertexToPixelHeader.hlsli"

//VS
#include "PixelShader3DHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit


// �萔�o�b�t�@�F�X���b�g0�Ԗ�(b0�Ə���)
cbuffer cbParam : register(b0)
{
	float4 g_color;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	// UV���W�ƃe�N�X�`�����Q�Ƃ��āA�œK�ȐF���擾����
	float4 TextureDiffuseColor;
   
	//float4 one = {1.0f,1.0f,1.0f,1.0f};
	float4 add = { 0.3f, 0.3f, 0.3f, 0.0f };
    
	float2 uv = PSInput.uv;
	TextureDiffuseColor = diffuseMapTexture.Sample(diffuseMapSampler, uv);

	
	float3 lightDir = PSInput.lightDir;

	
	float4 srcCol = g_common.material.diffuse * TextureDiffuseColor;
	
    // Norm��Light�̃x�N�g�������Ε����������Ă���قǐF�����邭�Ȃ�͂��B
    // �ł����Ε����̓��ς�0�ɋ߂Â��Ă����̂ŁA�t�ɂ���K�v������B
    //��0�`1�̃��m��1�`0�ɂ���ɂ́A-1��������0�`-1�ɂ������ƁA1�𑫂��΂悢�B
	float dotNormLight = dot(PSInput.normal, lightDir);
	dotNormLight = max(dotNormLight, -dotNormLight);
	srcCol.xyz *= (dotNormLight);
	return srcCol;
}