
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

static const float PI = 3.14159265359;

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float distributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float4 ps_main(VsOutput input) : SV_TARGET
{
    float4 col = albedoTex.Sample(samplerDefault, input.uv);
    float4 normal = normalTex.Sample(samplerDefault, input.uv);
    
    float4 fColor = float4(col.x, col.y, col.z, 1.0);
    
    return fColor;
}