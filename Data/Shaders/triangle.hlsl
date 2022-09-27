
cbuffer Globals
{
    matrix projection;
};

struct VsInput
{
    float3 position_local : POS;
    float3 color : COL;
};

struct VsOutput
{
    float4 position_clip : SV_POSITION;
    float4 color : COL;
};

VsOutput vs_main(VsInput input)
{
    VsOutput output;
    
    output.position_clip = float4(input.position_local, 1.0);
    output.position_clip = mul(output.position_clip, projection);

    output.color = float4(input.color, 1.0);

    return output;
}

float4 ps_main(VsOutput input) : SV_TARGET
{
    return input.color;
}