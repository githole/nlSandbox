/*
 * @brief VS/GS/PS共通ConstantBuffer。フレーム毎に一回だけ設定される事を想定している
 */
cbuffer Const8:register(b8)
{ 
	f4 sceneTime;       /* シーン中での経過時間 [0.0,1.0] */
	f4 sceneTotalTime;  /* 現在のシーンの総時間 */
	f4 Const16_pack[2];
};
/**
 * @brief zn/zf/aspect/fovyなどプロジェクション関連
 */
cbuffer Const9:register(b9)
{
	/* zf(x成分) */
	f4 zn;
	/* zf(x成分) */
	f4 zf;
	/* FOVY(x成分) */
	f4 fovy;
	/* バックバッファの解像度。(width/heightがx/y成分) */
	f4 resolution;
};

/**/
struct VO_EF_DEPTH_NORMAL
{
	f4 pos:SV_POSITION;
    f4 depth:TEXCOORD0;
    f4 normal:TEXCOORD1;
};
/**/
struct VO_POS_NORMAL_COLOR
{
	f4 Pos:SV_POSITION;
	f4 P:TEXCOORD0;
    f4 N:TEXCOORD1;
    f4 C:TEXCOORD2;
};
/**/
struct VO
{
    f4 pos:SV_POSITION;
    f4 col :COLOR0;
    f2 uv :TEXCOORD0;
};
struct CreateShadowVO
{
    f4 pos   : SV_POSITION;
    f4 depth : TEXCOORD0;
};
struct RefShadowVO
{
    f4 pos:SV_POSITION;
    f4 col :COLOR0;
    f2 uv :TEXCOORD0;
    f4 smuv :TEXCOORD1;
    f4 depth :TEXCOORD2;
};
struct ScreenVO
{
    f4 pos:SV_POSITION;
    f2 uv :TEXCOORD0;
};
struct DepthVO
{
    f4 pos   : SV_POSITION;
    f1 depth : TEXCOORD0;
};
struct NormalVO
{
    f4 pos  :SV_POSITION;
    f4 norm :COLOR0;
};
struct DepthNormalVO
{
    f4 pos  :SV_POSITION;
    f4 norm :COLOR0;
    f1 depth : TEXCOORD0;
};
struct GBVO
{
    f4 pos:SV_POSITION;
    f4 col:COLOR0;
    f2 uv :TEXCOORD0;
    f4 screenNormal :TEXCOORD1;
    f1 depth :TEXCOORD2;
    f2 verocity : TEXCOORD3;
};
struct HexSampleVO
{
    f4 pos:SV_POSITION;
    f4 depth :TEXCOORD0;
    f4 col :COLOR0;
};
struct HbaoVO
{
	f4 pos:SV_POSITION;
    f4 col :COLOR0;
    f4 depth :TEXCOORD0;
};
struct PO1
{
    f4 c0:SV_Target0;
};
struct PO2
{
    f4 c0:SV_Target0;
    f4 c1:SV_Target1;
};
struct PO3
{
    f4 c0:SV_Target0;
    f4 c1:SV_Target1;
    f4 c2:SV_Target2;
};

SamplerState SMP_LINER
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};
SamplerState SMP_LINER_CLAMP
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};
f4 AOGauss(Texture2D tex, Texture2D depth, f2 uv, f1 dispersion, f1 xy01)
{
    /**/
    uint w,h,lvl;
	tex.GetDimensions(0,w,h,lvl);
	/**/
    uv.x += 0.5*(1.0/w);
    uv.y += 0.5*(1.0/h);
	/**/
    //float total=0;
    const int weight = 8;
	
    //float tbl[weight*2];
	/*
    for(int i=0;i<weight;++i)
    {
        tbl[i] = exp(-0.5f*(i*i)/dispersion);
        if(0==i) {total += tbl[i];}
        else     {total += 2.0f*tbl[i];}
	}
	for(int i=0;i<weight;++i)
	{tbl[i]/=total;	} 
	*/

	const f1 eX = 1.0 - xy01;
	const f1 eY = xy01;
	float total=1;
	f4 col = tex.Sample(SMP_LINER,uv);
	const f4 dep = depth.Sample(SMP_LINER,uv);
	//const f1 falloff = sqrt(dep.x);
	const f1 thrhold = 0.4;// * falloff;
    for( int i=1;i<weight;++i)
    {
		const f2 lt = uv+f2(( 2.0*i)/w*eX,( 2.0*i)/w*eY);
		const f2 rt = uv+f2((-2.0*i)/w*eX,(-2.0*i)/w*eY);
		f1 ld = (abs(depth.Sample(SMP_LINER,lt).y - dep.y) > thrhold ? 0.0 : 1.0);
		f1 rd = (abs(depth.Sample(SMP_LINER,rt).y - dep.y) > thrhold ? 0.0 : 1.0);
		f1 sc = exp(-0.5f*(i*i)/dispersion);
		ld *=sc;
		rd *=sc;
		total += ld + rd;
		col+= (tex.Sample(SMP_LINER,lt) * f4(ld,ld,ld,ld));
		col+= (tex.Sample(SMP_LINER,rt) * f4(rd,rd,rd,rd));
	}
	col /= total;
    /**/
    return col;
}

/**
 * @brief 横方向ガウスブラー
 * @param Texture2D tex 参照イメージ
 * @param f2 uv 参照箇所
 * @param f1 dispression ぼやけ具合
 */
f4 gaussX(Texture2D tex, f2 uv, f1 dispersion)
{
    /**/
    uint w,h,lvl;
	tex.GetDimensions(0,w,h,lvl);
	/**/
    //uv.x += 0.5*(1.0/w);
    //uv.y += 0.5*(1.0/h);
	/**/
    float total=0;
    const int weight = 8;
    float tbl[weight];
    for(int i=0;i<weight;++i)
    {
        tbl[i] = exp(-0.5f*(i*i)/dispersion);
        if(0==i) {total += tbl[i];}
        else     {total += 2.0f*tbl[i];}
	}
	for(int i=0;i<weight;++i)
	{tbl[i]/=total;	} 
	/**/
	f4 col = tbl[0]*tex.Sample(SMP_LINER_CLAMP,uv);
    for( int i=1;i<weight;++i)
    {col+=tbl[i]*(tex.Sample(SMP_LINER_CLAMP,uv+f2((2.0*i)/w,0))+tex.Sample(SMP_LINER_CLAMP,uv+f2((-2.0*i)/w,0)));}
    /**/
    return col;
}
/**
 * @brief 縦方向ガウスブラー
 * @param Texture2D tex 参照イメージ
 * @param f2 uv 参照箇所
 * @param f1 dispression ぼやけ具合
 */
f4 gaussY(Texture2D tex,f2 uv,f1 dispersion )
{
    /**/
    uint w,h,lvl;
	tex.GetDimensions(0,w,h,lvl);
	/**/
    //uv.x += 0.5*(1.0/w);
    //uv.y += 0.5*(1.0/h);
	/**/
    float total=0;
    const int weight = 8;
    float tbl[weight];
    for(int i=0;i<weight;++i)
    {
        tbl[i] = exp(-0.5f*(i*i)/dispersion);
        if(0==i) {total += tbl[i];}
        else     {total += 2.0f*tbl[i];}
	}
	for(int i=0;i<weight;++i)
	{tbl[i]/=total;	} 
	/**/
	f4 col = tbl[0]*tex.Sample(SMP_LINER_CLAMP,uv);
    for( int i=1;i<weight;++i)
    {col+=tbl[i]*(tex.Sample( SMP_LINER_CLAMP,(uv+f2(0,(2.0*(f1)i)/h)))+tex.Sample( SMP_LINER_CLAMP,uv+f2(0,(-2.0*(f1)i)/h)));}
    /**/
    return col;
}

/**
 * 乱数生成
 * via http://www.ozone3d.net/blogs/lab/20110427/glsl-random-generator/
 */
double rand(float2 co)
{
    return frac(sin(dot(co.xy ,float2(12.9898,78.233))) * 43758.5453);
}
