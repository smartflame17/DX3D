struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

cbuffer cBuf
{
    matrix transform;
};

VSOut main( float2 pos : POSITION, float3 color : COLOR )
{
    VSOut vso;
    vso.pos = mul(transform, float4(pos.x, pos.y, 0.0f, 1.0f));
    vso.color = color;
    return vso;
}