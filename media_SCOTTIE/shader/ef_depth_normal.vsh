/**
* @brief ���`�[�x�ƃX�N���[���X�y�[�X�@�����o�͂���
*        ef_depth_normal.psh�ƍ��킹�ė��p����
* RT0: R16G16B16A16_FLOAT(R:�[�x GB:�@��32bit)
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
{f4x4 vp;}
/**/
VO_EF_DEPTH_NORMAL main(
          f4 iP:P,
          f4 iN:N,
          f4 iC:C,
          uint iM: M )
{
    VO_EF_DEPTH_NORMAL ret;
    ret.pos = mul(iP,mul(world,vp));
    ret.depth  = ret.pos;
    ret.normal = ret.pos;/* ������ */
    return ret;
}
