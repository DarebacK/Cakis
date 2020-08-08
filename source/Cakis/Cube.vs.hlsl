#pragma pack_matrix(row_major)

cbuffer Transformation
{
  float4x4 transform;
  float4 color;
};

struct Output
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

Output main(
  float3 position : POSITION
)
{
  Output output;
  output.position = mul(float4(position, 1.0f), transform);
  output.color = float4(color);
  return output;
}