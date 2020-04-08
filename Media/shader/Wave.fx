texture2D texSource;
float4 materialColor;
int colorOpMode;
int alphaOpMode;
float2 uvScale;

sampler2D sourceSampler = sampler_state
{
    Texture = <texSource>;
    AddressU = Wrap;
    AddressV = Wrap;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

float4 ps_Combine ( float2 uv0 : TEXCOORD0,
					float2 uv1 : TEXCOORD1) : COLOR
{
	//uv0.x *= uvScale.x;
	//uv0.y *= uvScale.y;	
	
	float4 src = tex2D(sourceSampler, uv0);
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
		ZEnable = false;
		ZWriteEnable = false;
		DestBlend = 2;
		
		PixelShader = compile ps_2_b ps_Combine();
	}
}