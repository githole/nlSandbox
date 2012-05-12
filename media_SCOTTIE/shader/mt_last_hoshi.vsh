/**
* @brief �e�X�g�p�Ɏw�肵���J���[��Diffuse�o�͂�����B
*        db_diffuse.psh�ƍ��킹�ė��p����
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
    r.N = mul(iN,world); /* �X�P�[�����c��ł���ꍇ�͗ǂ��Ȃ����C�ɂ��Ȃ� */
    //r.C = abs(iP.x) + abs(iP.y) + abs(iP.z);
    r.C = f4(iP.xyz,1);
    return r;
}
