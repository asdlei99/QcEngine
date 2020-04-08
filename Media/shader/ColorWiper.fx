
texture2D texSource;

sampler2D sourceSampler = sampler_state
{	
	Texture = <texSource>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
    AddressV = CLAMP;
};

float4 ColorWiper_ps( float2 uv : TEXCOORD0 ) : COLOR
{
	float4 src = tex2D(sourceSampler, uv);
	float coef = (src.r+src.g+src.b)/3.f;
	src.rgb = coef;
	
	return src;
}

// technique
//------------------------------------------------------------------
technique Effect
{
	pass P0
	{		
		PixelShader = compile ps_2_b ColorWiper_ps();
	}
}