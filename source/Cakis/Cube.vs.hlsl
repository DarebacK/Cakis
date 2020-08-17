#pragma pack_matrix(row_major)

struct Output
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

Output main(
  float3 position : POSITION,
  float4x4 transform : INSTANCE_TRANSFORM,
  float4 color : INSTANCE_COLOR
)
{
  Output output;
  output.position = mul(float4(position, 1.0f), transform);
  output.color = float4(color);
  return output;
}