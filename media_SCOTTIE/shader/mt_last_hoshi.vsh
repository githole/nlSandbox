/**
* @brief テスト用に指定したカラーでDiffuse出力をする。
*        db_diffuse.pshと合わせて利用する
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
{ f4x4 proj; }
/**/
VO_POS_NORMAL_COLOR main(
          f4 iP:P,
          f4 iN:N,
          f4 iC:C,
          uint iM: M )
{
    VO_POS_NORMAL_COLOR r;
    r.P = mul(iP,world);
    r.Pos.xyz = r.P.xzy;
    r.Pos.w = r.P.w;
    r.N = mul(iN,world); /* スケールが歪んでいる場合は良くないが気にしない */
    //r.C = abs(iP.x) + abs(iP.y) + abs(iP.z);
    r.C = f4(iP.xyz,1);
    return r;
}
