
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

Texture2D albedoTex : register(t1);
Texture2D normalTex : register(t2);

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

    return output;
}

float4 ps_main(VsOutput input) : SV_TARGET
{
    float3 albedo = albedoTex.SampleLevel(samplerDefault, input.uv, 0).rgb;
    float3 normal = normalTex.SampleLevel(samplerDefault, input.uv, 0).rgb;

    const float3 normalSpace = normalize(normal * 2.0 - 1.0);
    const float3 inputNormal = normalize(input.normal);

    const float3 lightDir = lightPositions[0] - input.worldPos;
    const float3 Wi = normalize(lightDir);
    const float3 radiance = lightColours[0];// *l.brightness;

    const float NdotL = saturate(dot(inputNormal, Wi));

    float3 diffuse = albedo * radiance * NdotL;

    float4 fColor = float4(diffuse.x, diffuse.y, diffuse.z, 1.0);
    
    return fColor;
}