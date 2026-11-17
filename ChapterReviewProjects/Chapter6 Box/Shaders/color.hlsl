cbuffer cbPerObject: register(b0){
    float4x4 gWorldViewProj;
};

struct MeshData
{
    float3 Pos: POSITION;
    float4 Color: COLOR;
}

struct Interpolators
{
    float4 Pos: SV_POSITION;
    float4 Color: COLOR; 
}

Interpolators VS(MeshData i)
{
    Interpolators o;
    
    o.pos = mul(float4(i.Pos, 1.0f), gWorldViewProj);
    
    o.Color = i.Color;
    
    return o;
}

float4 PS(Interpolators i) : SV_Target
{
    return i.Color;
}