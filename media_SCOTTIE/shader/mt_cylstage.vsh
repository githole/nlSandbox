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

static const f4 col[] = {
	f4(.9,.9,.9,1),
//	f4(115/255.0,255/255.0,32/255.0,1)
	f4(99/255.0,45/255.0,255/255.0,1)
//	f4(29/255.0,228/255.0,232/255.0,1)
};

VO_POS_NORMAL_COLOR main(
          f4 iP:P,
          f4 iN:N,
          f4 iC:C,
          uint iM: M )
{
	float s = sin(sceneTime.x*8*PI);
	float note = max(0,s*s);
	
    VO_POS_NORMAL_COLOR r;
    r.P = mul(iP,world);
    r.Pos = mul(r.P,vp);
    r.N = normalize(mul(iN,world)); /* スケールが歪んでいる場合は良くないが気にしない */
    r.C = col[iM] + col[1] * ((1-iM) * note * 0.5 + iM * note + 0.1);
    r.C.a = iM;
    return r;
}
