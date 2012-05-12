/** 
* ==========================================
* @brief 深度の作成を行う
* ==========================================
*/
/* REPLACE DepthVO,DepthVO */

/* オブジェクト毎 */
cbuffer Const0 : register(b0)
{ f4x4 c4World; }
cbuffer Const1 : register(b1)
{ f4x4 reser; }
/* オブジェクトグループ全体 */
cbuffer Const2 : register(b2)
{ f4x4 c0ViewProj; }
cbuffer Const3 : register(b3)
{ f4x4 c1View;}
cbuffer Const4 : register(b4)
{ f4x4 res2; }
/**/
DepthVO main(
                f4 in_pos:P,
                f4 in_normal:N,
                f4 in_col:C,
                uint in_matId: M )
{
    DepthVO ret;
    f4x4 wvp = mul(c4World,c0ViewProj);
    ret.pos   = mul(in_pos,wvp );
    /**/
    f4 dist = mul(in_pos,mul(c4World,c1View));
    ret.depth = -dist.z/dist.w;
    return ret;
}
