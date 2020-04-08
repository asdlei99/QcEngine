
texture2D texSource;
float4 materialColor;
int colorOpMode;
int alphaOpMode;

sampler2D sourceSampler = sampler_state
{	
	Texture = <texSource>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
    AddressV = CLAMP;
};

float4 default_ps( float2 uv : TEXCOORD0 ) : COLOR
{
	float4 src = tex2D(sourceSampler, uv);
	if (colorOpMode == 1)
		src.rgb = 0;
	else if (colorOpMode == 2)
		src.rgb = materialColor.rgb;
	else if (colorOpMode == 4)
		src.rgb *= materialColor.rgb;
	else if (colorOpMode == 5)
	{
		src.rgb *= materialColor.rgb;
		src.rgb *= 2.0;
	}
	else if (colorOpMode == 6)
	{
		src.rgb *= materialColor.rgb;
		src.rgb *= 4.0;
	}
	else if (colorOpMode == 7)
		src.rgb += materialColor.rgb;
	else
	{
		//No Impl
		src.rgb = 0;
	}
		
	if (alphaOpMode == 1)
		src.a = 0;
	else if (alphaOpMode == 2)
		src.a = materialColor.a;
	else if (alphaOpMode == 4)
		src.a *= materialColor.a;
	else if (alphaOpMode == 5)
	{
		src.a *= materialColor.a;
		src.a *= 2.0;
	}
	else if (alphaOpMode == 6)
	{
		src.a *= materialColor.a;
		src.a *= 4.0;
	}
	else if (alphaOpMode == 7)
		src.a += materialColor.a;
	else
	{
		//No Impl
		src.a = 0;
	}
	return src;
}

// technique
//------------------------------------------------------------------
technique Effect
{
	pass P0
	{		
		PixelShader = compile ps_2_b default_ps();
	}
}