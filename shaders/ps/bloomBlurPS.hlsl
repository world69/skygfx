// Bloom separable blur: 5-tap gaussian (weights sum to 1), no loops so it
// stays ps_2_a compatible.
uniform sampler2D tex : register(s0);
uniform float4 off : register(c0);	// xy = one texel in the blur direction

struct PS_INPUT
{
	float3 texcoord0	: TEXCOORD0;
};

float4
main(PS_INPUT IN) : COLOR
{
	float2 uv = IN.texcoord0.xy;
	float3 c =  tex2D(tex, uv).rgb * 0.227027f;
	c += tex2D(tex, uv + off.xy * 1.3846154f).rgb * 0.3162162f;
	c += tex2D(tex, uv - off.xy * 1.3846154f).rgb * 0.3162162f;
	c += tex2D(tex, uv + off.xy * 3.2307692f).rgb * 0.0702703f;
	c += tex2D(tex, uv - off.xy * 3.2307692f).rgb * 0.0702703f;
	return float4(c, 1.0f);
}
