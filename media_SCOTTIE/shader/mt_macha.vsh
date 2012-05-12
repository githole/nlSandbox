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
f4(242/255.0, 230/255.0,196/255.0,1),/* 肌 */
f4(0/255.0, 0/255.0,0/255.0,1),   /* 目、文字 */
f4(255/255.0, 200/255.0,166/255.0,1),/* 服 */
f4(79/255.0,109/255.0,147/255.0,1),
f4(173/255.0,173/255.0,173/255.0,1),/* はちまき */
f4(83/255.0, 63/255.0,67/255.0,1),/* スカート */
f4(83/255.0,63/255.0,67/255.0,1),
f4(173/255.0,173/255.0,173/255.0,1),
};
VO_POS_NORMAL_COLOR main(
          f4 iP:P,
          f4 iN:N,
          f4 iC:C,
          uint iM: M )
{
    VO_POS_NORMAL_COLOR r;
    r.P = mul(iP,world);
    r.Pos = mul(r.P,vp);
    r.N = normalize(mul(iN,world)); /* スケールが歪んでいる場合は良くないが気にしない */
    r.C = col[iM];
    return r;
}
