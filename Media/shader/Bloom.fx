texture txSourceColor;
texture txCombineColor;

sampler2D Texture0 =
sampler_state
{
    Texture = <txSourceColor>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D Texture1 =
sampler_state
{
    Texture = <txCombineColor>;
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Linear;
    MipFilter = Linear;
};

float Hue = 0;

float4 ps_HueChange( float2 uv : TEXCOORD0 ) : COLOR
{
	float4 color = tex2D(Texture0, uv);

    float fMax = max(color.r, color.g);
    fMax = max(fMax, color.b);
    float fMin = min(color.r, color.g);
    fMin = min(fMin, color.b);
        
    float H = Hue;
    
    if (H <= 0) H += 360;
    if (H > 360) H -= 360;
  
    float l = (fMax+fMin) * 0.5f;
    float s;
    if (l == 0 || fMax == fMin)
       s = 0;
    else if ( l > 0 && l <= 0.5f)
       s = (fMax-fMin)/(2*l);
    else
       s = (fMax-fMin)/(2- 2*l);
       
    if (s == 0)
       color.rgb = l;
    else
    {
       float q,p,k,t;
       if (l < 0.5f)
          q = l * (1+s);
       else
          q = l + s - l*s;
       
       p = 2*l - q;
       
       k = H / 360.0f;
       const float w = 1.0f/3.0f;
       for (int i=0, j=1; i<3; i++,j--)
       {
          color[i] = k + w*j;
          if (color[i] < 0) color[i] += 1;
          if (color[i] > 1) color[i] -= 1;
          
          if (color[i] < 1.0f/6.0f)
             color[i] = p + (q-p) * 6 * color[i];
          else if (color[i] < 0.5f)
             color[i] = q;
          else if (color[i] < 2.0f/3.0f)
             color[i] = p + (q-p) * 6 * (2.0f/3.0f - color[i]);
          else
             color[i] = p;
       }
       
    }
    
    return color;
}

#define NUM_SAMPLES 20
float Density = 0.5f;
float Weight = 0.9f;
float Decay = 0.8f;

float4 ps_Bloom ( float2 uv : TEXCOORD0) : COLOR
{
	half2 deltaTexCoord = (uv - 0.5);   
   
	deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;   
  
	float3 color = tex2D(Texture0, uv);  
   
	half illuminationDecay = 1;
		
	for (int i = 0; i < NUM_SAMPLES; i++)  
	{ 
		uv -= deltaTexCoord;
		
		half3 sample = tex2D(Texture0, uv); 
		
		sample *= illuminationDecay * Weight;   
		
		color += sample;   
		  
		illuminationDecay *= Decay;  
	}   
	
	return float4(color*.7f, 1); 
}

float4 ps_Combine1 ( float2 uv0 : TEXCOORD0,
					float2 uv1 : TEXCOORD1) : COLOR
{
	return tex2D(Texture0, uv0) + tex2D(Texture1, uv1)*.75f;
}

// technique
//------------------------------------------------------------------
technique Effect
{
	pass P0
	{		
		ZEnable = false;
		ZWriteEnable = false;
		
		PixelShader = compile ps_2_b ps_HueChange();
	}
	
	pass P1
	{
		PixelShader = compile ps_2_b ps_Bloom();
	}
	
	pass P2
	{
		ALPHATESTENABLE = TRUE;
		ALPHAFUNC = GREATEREQUAL;
		ALPHAREF = 0xa0;
		
		PixelShader = compile ps_2_b ps_Combine1();
	}
}