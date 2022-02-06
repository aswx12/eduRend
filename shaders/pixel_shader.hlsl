
Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);

SamplerState texSampler : register(s0);

cbuffer LightCamBuffer : register(b0)
{
	float4 cameraPos;
	float4 lightPos;
};

cbuffer MateriaBuffer : register(b1)
{
	float4 Ka, Kd, Ks;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
	float3 PosWorld : PosWorld;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	//return float4(input.Normal * 0.5 + 0.5, 1);

// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//return float4(input.TexCoord, 0, 1);

	float3 N = normalize(input.Normal);
	float3 L = normalize(lightPos.xyz - input.PosWorld); //light vector
	float3 V = normalize(cameraPos.xyz - input.PosWorld); //view vector

	float4 diffColor = texDiffuse.Sample(texSampler, input.TexCoord);

	float3 R = normalize(reflect(-L, N)); //L reflect in N
	float a = 200;

	float4 Id = max(diffColor * dot(L, N),0);
	float4 SpecDot = max(dot(R, V),0);
	float4 SpecPow = pow(SpecDot, a);
	float4 Is = Ks * SpecPow;
	float4 I = Ka * 0.5 + Id + Is ;
	return I;
}