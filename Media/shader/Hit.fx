
texture texSource;
float gfLuminance = 0.f;

sampler sourceSampler = sampler_state
{	
	Texture = <texSource>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = CLAMP;
    AddressV = CLAMP;
};

float3 vLuminance = float3( 0.3f, 0.59f, 0.11f );

float luminance( float3 c )
{
	return dot( c, vLuminance );
}

float4 hit_ps( float2 uv : TEXCOORD0 ) : COLOR
{
	float4 color = tex2D(sourceSampler, uv);
	
	if(color.a >= 1.f/255.0f)
	{
		float temp = luminance(color.rgb);
		if (temp < gfLuminance)
		{
			if (temp > 0.05)
				color.rgba *= gfLuminance/temp;
// 			else
// 				color.rgb = vLuminance;
		}
	}
	return color;
}

// technique
//------------------------------------------------------------------
technique Effect
{
	pass P0
	{
		CullMode = none;
		ZEnable = false;
		ZWriteEnable = false;
		
		PixelShader = compile ps_2_b hit_ps();
	}
}