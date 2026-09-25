// Final screen pass: chromatic aberration, bloom add, exposure, filmic tone
// map, grading matrix (YCbCr tweak or identity), vignette, PS2-style
// grain+scanlines and optional 15-bit dithering.
// All effects are gated by uniform constants so the pass is a no-op when
// everything is disabled (identity grading, exposure 1, rest zero).
uniform sampler2D scene : register(s0);
uniform sampler2D bloom : register(s1);
uniform sampler2D dither : register(s2);
uniform sampler2D grain : register(s3);

uniform float4 gradeR : register(c0);
uniform float4 gradeG : register(c1);
uniform float4 gradeB : register(c2);
// c3: x = exposure, y = tone map white point (0 = off),
//     z = bloom intensity, w = dither strength (0 = off)
uniform float4 params : register(c3);
// c4: x = vignette strength (0 = off), y = chromatic aberration strength
//     (0 = off), z = grain+scanline strength (0 = off)
uniform float4 fx : register(c4);

struct PS_INPUT
{
	float3 texcoord0	: TEXCOORD0;
};

// Narkowicz filmic approximation, input expected in [0, 1]
float3
filmic(float3 x)
{
	return (x * (2.51f * x + 0.03f)) / (x * (2.43f * x + 0.59f) + 0.14f);
}

float4
main(PS_INPUT IN) : COLOR
{
	float2 uv = IN.texcoord0.xy;

	// chromatic aberration: shift red/blue a fixed 1-2px (cheap, subtle)
	float3 c;
	if(fx.y > 0.0f)
	{
		float2 d = float2(fx.y * 0.002f, 0.0f);
		c.r = tex2D(scene, uv + d).r;
		c.g = tex2D(scene, uv).g;
		c.b = tex2D(scene, uv - d).b;
	}
	else
		c = tex2D(scene, uv).rgb;

	// bloom
	if(params.z > 0.0f)
		c += tex2D(bloom, uv).rgb * params.z;

	// exposure
	if(params.x != 1.0f)
		c *= params.x;

	// tone map
	if(params.y > 0.0f)
		c = filmic(c / params.y);

	// grading (identity unless the YCbCr tweak is enabled)
	float4 cc = float4(c, 1.0f);
	float4 o;
	o.r = dot(gradeR, cc);
	o.g = dot(gradeG, cc);
	o.b = dot(gradeB, cc);
	o.a = 1.0f;

	// vignette: darken the corners
	if(fx.x > 0.0f)
	{
		float2 v = uv - 0.5f;
		o.rgb *= saturate(1.0f - dot(v, v) * (fx.x * 1.8f));
	}

	// PS2-style grain + 2px scanlines (pre-baked texture, s3)
	if(fx.z > 0.0f)
	{
		float g = tex2D(grain, uv).r;
		o.rgb += (g - 0.5f) * 2.0f * fx.z * 0.25f;
	}

	// PS2-style dither: quantize to 32 levels per channel with blue noise
	if(params.w > 0.0f)
	{
		float d = tex2D(dither, uv).r;
		o.rgb = floor(o.rgb * 32.0f + d - 0.5f) / 32.0f;
	}

	return o;
}
