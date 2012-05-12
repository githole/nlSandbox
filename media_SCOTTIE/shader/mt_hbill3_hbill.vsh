/**
* @brief 
* RT0: R8G8B8A8
* C0:  World行列
* C1:  未使用
* C2:  ViewProj行列
*/
/* REPLACE VO_EF_DEPTH_NORMAL,VO_EF_DEPTH_NORMAL */
/* オブジェクト毎 */
cbuffer Const0 : register(b0)
{ f4x4 world; }
/* オブジェクトグループ全体 */
cbuffer Const2 : register(b2)
{ f4x4 vp; }
/**/
struct GSIn
{
	float4 P : POS;
	float4 N : NORMAL;
	float4 C : TEXCOORD0;
	float4 OP : TEXCOORD1;
};

GSIn main(
          f4 iP:P,
          f4 iN:N,
          f4 iC:C,
          uint iM: M )
{
    GSIn r;
    r.P = mul(iP,world)  ;// mul(world,vp));
    r.N = normalize(mul(iN,world)); /* スケールが歪んでいる場合は良くないが気にしない */
    r.C = iC;
    r.C.x = iM;
    r.OP = iP;
    return r;
}
