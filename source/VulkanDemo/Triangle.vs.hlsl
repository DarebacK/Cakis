struct Output
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

Output main(float2 position : POSITION, 
            float3 color : COLOR)
{
  Output output;
  output.position = float4(position, 0.0f, 1.0f);
  output.color = float4(color, 1.0f);
  return output;
}