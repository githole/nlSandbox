/**
* @brief 
* RT0: R8G8B8A8
* C0:  World�s��
* C1:  ���g�p
* C2:  ViewProj�s��
*/
/* REPLACE VO_EF_DEPTH_NORMAL,VO_EF_DEPTH_NORMAL */
/* �I�u�W�F�N�g�� */
cbuffer Const0 : register(b0)
{ f4x4 world; }
/* �I�u�W�F�N�g�O���[�v�S�� */
cbuffer Const2 : register(b2)
{ f4x4 vp; }
/**/
static const f4 col[] = {
f4(242/255.0, 230/255.0,196/255.0,1),/* �� */
f4(0/255.0, 0/255.0,0/255.0,1),   /* �ځA���� */
f4(255/255.0, 200/255.0,166/255.0,1),/* �� */
f4(79/255.0,109/255.0,147/255.0,1),
f4(173/255.0,173/255.0,173/255.0,1),/* �͂��܂� */
f4(83/255.0, 63/255.0,67/255.0,1),/* �X�J�[�g */
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
    r.N = normalize(mul(iN,world)); /* �X�P�[�����c��ł���ꍇ�͗ǂ��Ȃ����C�ɂ��Ȃ� */
    r.C = col[iM];
    return r;
}
