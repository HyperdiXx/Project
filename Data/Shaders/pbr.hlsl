
#include "LightingCommon.hlsl"

cbuffer Globals
{
    matrix vp;
    matrix model;
    matrix invModel;
    float4 lightPositions[4];
    float4 lightColours[4];
    float4 camPos;
    float4 customData;
};

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D metallRoghnessTex : register(t2);

SamplerState samplerDefault : register(s1);

struct VsInput
{
    float3 pos : POS;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 uv : UV;
};

struct VsOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD1;

    float3x3 tangentBasis : TBASIS;
};

VsOutput vs_main(VsInput input)
{
    VsOutput output;
    
    output.position = float4(input.pos, 1.0);
    output.position = mul(output.position, model);
    output.worldPos = output.position.xyz;

    output.position = mul(output.position, vp);

    output.normal = input.normal;       
    output.uv = input.uv;

    float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
    output.tangentBasis = mul((float3x3)model, transpose(TBN));

    return output;
}

float4 ps_main(VsOutput input) : SV_TARGET
{
    const float3 albedo = albedoTex.SampleLevel(samplerDefault, input.uv, 0).rgb;
    const float3 normal = normalTex.SampleLevel(samplerDefault, input.uv, 0).rgb;

    const float2 mr = metallRoghnessTex.SampleLevel(samplerDefault, input.uv, 0).gb;

    const float3 normalSpace = normalize(normal * 2.0 - 1.0);
    const float3 N = normalize(mul(input.tangentBasis, normalSpace));

    const float3 Li = normalize(lightPositions[0] - input.worldPos);
    const float3 Lo = normalize(camPos - input.worldPos);
    
    const float3 radiance = lightColours[0];// *l.brightness;
    
    float NdotL = max(0.0, dot(N, Li));
    float3 Lr = 2.0 * NdotL * N - Lo;
    float3 F0 = lerp(Fdielectric, albedo, mr.y);

    float3 directLight = 0.0;

    float3 brdfTerm = brdf(Li, Lo, N, albedo, radiance, mr.x, mr.y, F0);
   
    directLight = brdfTerm;

    // IBL part
    float3 ambientLight = 0.0;

    return float4(directLight, 1.0);
} 