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
VO_POS_NORMAL_COLOR main(
          f4 iP:P,
          f4 iN:N,
          f4 iC:C,
          uint iM: M )
{
    VO_POS_NORMAL_COLOR r;
    f4 p = iP;
    p.x *= resolution.y / resolution.x;
    p.xyz *= (0.2 + 0.02);
    p.x = -p.x;
    p -= f4(-0.2+0.01,0.08,0,0);
    r.P = r.Pos = p;
    r.N = f4(0,0,1,0);
    r.C = iC;
    return r;
}
