#pragma pack_matrix(row_major)

static const float colorThresholdMin = 0.3f;
static const float colorThresholdMax = 0.4f;

float4 main(
  float3 originalPosition : POSITION,
  float4 position : SV_POSITION,
  float4 color : COLOR
) : SV_TARGET
{
  float xStep = smoothstep(colorThresholdMin, colorThresholdMax, abs(originalPosition.x));
  float yStep = smoothstep(colorThresholdMin, colorThresholdMax, abs(originalPosition.y));
  float zStep = smoothstep(colorThresholdMin, colorThresholdMax, abs(originalPosition.z));
  float modifier = 1.f - smoothstep(1.75f, 2.f, xStep + yStep + zStep);
  return modifier * color;
}