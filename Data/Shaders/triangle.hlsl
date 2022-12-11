
cbuffer Globals
{
    matrix projection;
};

Texture2D albedoTex : register(t1);
SamplerState samplerDefault : register(s1);

struct VsInput
{
    float3 position_local : POS;
    //float3 color : COL;
    float2 uv : UV;
};

struct VsOutput
{
    float4 position_clip : SV_POSITION;
    //float4 color : COL;
    float2 uv : UV;
};

VsOutput vs_main(VsInput input)
{
    VsOutput output;
    
    output.position_clip = float4(input.position_local, 1.0);

    //output.position_clip = mul(output.position_clip, model);
    //output.position_clip = mul(output.position_clip, _mvp);

    output.position_clip = mul(output.position_clip, projection);

    //output.color = float4(input.color, 1.0);
    output.uv = input.uv;

    return output;
}

float4 ps_main(VsOutput input) : SV_TARGET
{
    float4 col = albedoTex.Sample(samplerDefault, input.uv);
    //input.color;
    return col;
}