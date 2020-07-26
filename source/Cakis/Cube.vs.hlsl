cbuffer Transformation
{
  float4x4 transform;
};

struct Output
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

Output main(
  float3 position : POSITION, 
  float3 color : COLOR
)
{
  Output output;
  output.position = mul(transform, float4(position, 1.0f));
  output.color = float4(color, 1.0f);
  return output;
}