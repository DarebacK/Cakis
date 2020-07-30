#pragma pack_matrix(row_major)

cbuffer Transformation
{
	float4x4 transform;
};

float4 main( float2 pos : POSITION ) : SV_POSITION
{
	return mul(float4(pos, 0.f, 1.f), transform);
}