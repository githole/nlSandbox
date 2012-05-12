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
    r.N = normalize(mul(iN,world)); /* �X�P�[�����c��ł���ꍇ�͗ǂ��Ȃ����C�ɂ��Ȃ� */
    r.C = iC;
    r.C.x = iM;
    r.OP = iP;
    return r;
}
