// Bloom bright pass: keep only the parts of the image above the threshold.
uniform sampler2D tex : register(s0);
uniform float4 params : register(c0);	// x = threshold

struct PS_INPUT
{
	float3 texcoord0	: TEXCOORD0;
};

float4
main(PS_INPUT IN) : COLOR
{
	float3 c = tex2D(tex, IN.texcoord0.xy).rgb;
	c = max(c - params.x, 0.0f) / max(1.0f - params.x, 0.001f);
	return float4(c, 1.0f);
}
