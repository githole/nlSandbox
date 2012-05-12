
cbuffer Const2 : register(b2)
{
  f4 c_Fov;
  f4 c_ZNear;
  f4 c_ZFar;
  f4 c_StrengthAngleR;//xyz  
}
/*
static const float g_R = 0.3;
static const float g_Angle = 20.0;
static const float g_Strength = 1.0f;
static const float g_ZNear = 0.1;
static const float g_ZFar = 1000.0;
static const float g_Fov = 20.89;*/

// const
static const float2 g_AOResolution = float2(640,360); /* AOテクスチャのサイズ */
static const float2 g_InvAOResolution = 1.0/g_AOResolution;


struct SSAO_VSOut
{
    float4 p : SV_Position;
    float2 u : TEXCOORD0;
    float4 FovyRFocal  : TEXCOORD1;
    float4 AngleLin    : TEXCOORD2;
    float4 UVToView    : TEXCOORD3;
};

SSAO_VSOut main(uint i:SV_VertexID)
{
	SSAO_VSOut r;
	float FovyRad = c_Fov;//3.141592*c_Fov/180.0;
	float2 FocalLen = float2( 1.0/tan(FovyRad*0.5f)*(g_AOResolution.x/g_AOResolution.y),
					1.0f / tan(FovyRad * 0.5f) );
	const float2 InvF =1.0/FocalLen;
	float2 UVToViewA = float2(2.0f*InvF.x,-2.0f*InvF.y);
	float2 UVToViewB = float2(-1.0f*InvF.x,1.0f*InvF.y);

	r.FovyRFocal = float4(FovyRad, c_StrengthAngleR.z, FocalLen);
	r.UVToView = float4(UVToViewA,UVToViewB);

	//float g_R2 = g_R*g_R;
	//float g_NegInvR2 = -1.0/g_R2;
	
	f1 agl = c_StrengthAngleR.y;
	f1 AngleBias = PI*agl/180.0;
	/* g_AngleBias *//* g_TanAngleBias*/
	f1 zn = c_ZNear.x;
	f1 zf = c_ZFar.x;
	r.AngleLin = f4(
		AngleBias,
		tan(AngleBias),
		1.0f/zf - 1.0f/zn,
		1.0f/zn.x
	);

	// Vertex/UV
    r.p = float4((((int)i%2)*2-1),((int)i/2*2-1),0,1);
    r.u = float2(i%2,1-i/2);
    return r;
}