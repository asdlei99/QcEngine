

struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD0;
};
//是否提高精度
bool bHighPrecision = true;

texture heightmap_prev;

sampler HeightPrevSampler = sampler_state
{
	Texture = <heightmap_prev>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture heightmap_current;

sampler HeightCurrentSampler = sampler_state
{
	Texture = <heightmap_current>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

//
// Pass through vertex shader
//
VS_OUTPUT VS_Passthrough(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = float4(In.Position, 1.0f);
	Out.Texcoord = In.Texcoord;

	return Out;
}

float4x4 wvp_matrix;
//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position, 1.0f), wvp_matrix);
	Out.Texcoord = In.Texcoord;

	return Out;
}

// 
float4 EncodeHeightmap(float fHeight)
{
	float h = fHeight;
	float positive = fHeight > 0 ? fHeight : 0;
	float negative = fHeight < 0 ? -fHeight : 0;

	float4 color = 0;

	
	color.r = positive;
	color.g = negative;
	
	if ( bHighPrecision )
	{
	
		color.ba = frac(color.rg*256);
	
		color.rg -= color.ba/256.0f;
	}
	return color;
}


float DecodeHeightmap(float4 heightmap)
{
	float4 table;

	if ( bHighPrecision )
		table = float4(1.0f, -1.0f, 1.0f/256.0f, -1.0f/256.0f);
	else
		table = float4(1.0f, -1.0f, 0.0f, 0.0f);
	
	return dot(heightmap, table);
}

float DecodeHeightmap(sampler2D HeightmapSampler, float2 texcoord)
{
	float4 heightmap = tex2D(HeightmapSampler, texcoord);
	return DecodeHeightmap(heightmap);
}

float4 texture_size;
float  fDamping;
//
// Pixel Shader
//
float4 PS_Simulate(VS_OUTPUT In) : COLOR
{
	float3 offset[4] =
	{
		float3(-1.0f, 0.0f, 0.25f),
		float3( 1.0f, 0.0f, 0.25f),
		float3( 0.0f,-1.0f, 0.25f),
		float3( 0.0f, 1.0f, 0.25f),
	};	

	// float4(1.0f, -1.0f, 1.0f/256.0f, -1.0f/256.0f);
	float fHeightPrev = DecodeHeightmap(HeightPrevSampler, In.Texcoord);
	

	float fNeighCurrent = 0;
	for ( int i=0; i<4; i++ )
	{
		float2 texcoord = In.Texcoord + offset[i].xy * texture_size.xy;
		fNeighCurrent += (DecodeHeightmap(HeightCurrentSampler, texcoord) * offset[i].z);
	}

	float fHeight = fNeighCurrent * 2.0f - fHeightPrev;
	fHeight *= fDamping;
	float4 color = EncodeHeightmap(fHeight);
			
	return color;
}

float fNormalScale;

float4 PS_Normal(VS_OUTPUT In) : COLOR
{
	float2 offset[4] =
	{
		float2(-1.0f, 0.0f),
		float2( 1.0f, 0.0f),
		float2( 0.0f,-1.0f),
		float2( 0.0f, 1.0f),
	};	

	float fHeightL = DecodeHeightmap(HeightCurrentSampler, In.Texcoord + offset[0]*texture_size.xy);
	float fHeightR = DecodeHeightmap(HeightCurrentSampler, In.Texcoord + offset[1]*texture_size.xy);
	float fHeightT = DecodeHeightmap(HeightCurrentSampler, In.Texcoord + offset[2]*texture_size.xy);
	float fHeightB = DecodeHeightmap(HeightCurrentSampler, In.Texcoord + offset[3]*texture_size.xy);
	
	float3 n = float3(fHeightR - fHeightL,fHeightB - fHeightT,fNormalScale);
	float3 normal = (n + 1.0f) * 0.5f;
	
	return float4(normal.rgb, 1.0f);
}

float4 PS_Heightmap(VS_OUTPUT In) : COLOR
{
	float height = DecodeHeightmap(HeightCurrentSampler, In.Texcoord);
	return (height + 1.0f) * 0.5f;
}


float fForce;

float4 PS_Impulse(VS_OUTPUT In) : COLOR
{
	float4 color = EncodeHeightmap(fForce);
	return color;
}


texture NormalmapTex;

sampler2D NormalmapSampler = sampler_state
{
	Texture = <NormalmapTex>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture WaterTex;

sampler2D WaterSampler = sampler_state
{
	Texture = <WaterTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

float fTexcoordScale;

float4 PS_Water(VS_OUTPUT In) : COLOR
{
	float4 normalmap = tex2D(NormalmapSampler, In.Texcoord);
	float3 normal = (normalmap.rgb - 0.5f) * 2.0f;
	float2 texcoord = In.Texcoord + normal.xy * fTexcoordScale;
	float4 color = tex2D(WaterSampler, texcoord);
	return color;
}

// 1.渲染物体
technique AddImpulse
{
	pass p0 
	{
		//VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_Impulse();

		AlphaBlendEnable = TRUE;
		AlphaTestEnable = FALSE;
		SrcBlend = ONE;
		DestBlend = ONE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

// 2.下一帧的高度
technique WaterSimulation
{
	pass p0 
	{
		//VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Simulate();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}
// 3. 计算发现
technique ConvertNormal
{
	pass p0 
	{
		//VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Normal();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique Heightmap
{
	pass p0 
	{
		//VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Heightmap();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}
// 4. 画背景
technique Water
{
	pass p0 
	{
		//VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Water();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}