#pragma once

#include "shared/nlTypedef.h"
#include "shared/container/nlVector.hpp"
#include "shared/container/nlArray.hpp"
#include "shared/nlEngineContext.hpp"
#include "shared/math/nlMath.h"

/*SceneNodeType*/
enum nlSceneNode_t
{
    SNT_INVALID = 0,
    SNT_NOP,
    SNT_MAT_NOP,
    SNT_ADD,
    SNT_SUB,
    SNT_DIV,
    SNT_MUL,
    SNT_MINMAX,
    SNT_DIF,
    SNT_ROT2,
    SNT_TRANCE,
    SNT_TRANCE2,
    SNT_SCALE,
    //SNT_CLAMP,
    SNT_SIN,
    SNT_COS,
    SNT_COSSIN,
    SNT_ATAN,
    SNT_NORM,
    SNT_TAN,
    SNT_EXP,
    SNT_EXPSTEP,
    SNT_GAMMA,
    SNT_MOD,
    SNT_LENGTH,
    SNT_DISTANCE,
    SNT_REPEAT,
    //SNT_SMOOTHSTEP,
    SNT_CONSTV4,
    SNT_MAKEV4,
    SNT_SWIZZLE2,
    //SNT_BINARIZE,
    //SNT_BLEND,
    SNT_SCENETIME,
    SNT_SYSTEM_CONST,
    SNT_RANGE,
    SNT_SUBARRAY,
    //SNT_ADRESS,
    SNT_RANDOM,
    SNT_CURVE,
    SNT_OBJECT,
    SNT_DEBUG_CAMERA,
    SNT_DEBUG_CAMERA_POS,
    SNT_MATRIX_VIEW,
    SNT_MATRIX_PROJ,
    SNT_MATRIX_PROJ_ORTHO,
    SNT_MATRIX_WORLD,
    SNT_MATRIX_SCALE,
    SNT_MATRIX_TRANCE,
    SNT_MATRIX_ROT,
    SNT_MATRIX_ROTZ,
    SNT_MATRIX_MUL,
    SNT_MATRIX_INV,
    SNT_MATRIX_PACK,
    /* �ȉ������_�����O�p�m�[�h */
    SNT_BEGINRENDER,
    SNT_ENDRENDER,
    SNT_PASS,
    SNT_CLEARRT,
    SNT_CLEARDS,
    SNT_SET_Z_STATE,
    SNT_SET_ALPHA_STATE,
    SNT_SETRENDERTARGET,
    SNT_SET_CB,
    SNT_SET_TEX,
    SNT_SET_TEX_RT,
    SNT_RENDER_SCREEN2,
    SNT_RENDER_SCREEN3,
    SNT_DRAWGROUP,
};
const int SCENENODE_INPUT_NUM = 8;
const int SCENENODE_PROP_NUM  = 8;

enum nlEdgeType
{
    NL_ET_None,
    NL_ET_Vec4,
    NL_ET_Matrix,
    NL_ET_Render,
};

/* Scene Node */
struct nlSceneNode
{
    nlSceneNode_t  type_;
    nlArray<unsigned int,SCENENODE_INPUT_NUM> prevNode_;
    nlArray<nlFloat32,SCENENODE_PROP_NUM> prop_;
    unsigned int numValve_;
    nlEdgeType outEdgeType;
};

/*  */
struct nlNodeValveArray
{
public:
    nlNodeValveArray();
    nlNodeValveArray(unsigned int lenAsVec4 );
    ~nlNodeValveArray();
    nlVec4 atAsVec4( unsigned int idx ) const;
    const nlMatrix atAsMatrix( unsigned int idx )const;
    const nlMatrix* atAsMatrix2( unsigned int idx )const;
    void writeAsVec4( unsigned int idx, const nlVec4& value );
    void writeAsMatrix( unsigned int idx, const nlMatrix& value );
    void operator=(const nlNodeValveArray& other);
    unsigned int sizeAsVec4()const;
    unsigned int sizeAsMatrix()const;
    void resizeAsVec4( unsigned int newLen );
    void resizeAsMatrix( unsigned int newLen );
    bool empty()const
    { return !lenAsVec4_; }
private:
    nlNodeValveArray( const nlNodeValveArray& other );
private:
    unsigned int lenAsVec4_;
    nlVec4* array_;
};
/* �v�Z�r���p�̃o�b�t�@ */
struct ProcessInfoPerNode
{
    /* ���̃m�[�h�̉��Z���� */
    nlNodeValveArray output_;
    /* �������s��ꂽ���H */
    bool isProcessed_;
};

/* Scene Map */
typedef nlArray<nlSceneNode,MAX_SCENENODE_NUM> SceneMap;
/* scene Data */
struct nlScene
{
    /* �V�[���t���[���� */
    unsigned int sceneTimeInMs;
    /* �V�[���}�b�v�� */
    unsigned int numSceneMap_;
    /* �V�[���}�b�v */
    SceneMap sceneMaps_;
};
/**/
typedef nlArray<nlScene,MAX_SCENE_NUM> SceneArray;
typedef nlArray<ProcessInfoPerNode,MAX_SCENENODE_NUM> ProcessBuffer;

/**
 * @brief �V�[��������
 */
void nlScene_init();
/**
 * @brief �V�[��������
 *        �V�[��Array���O������w�肷��ꍇ�͂�����̃o�[�W�����𗘗p����B
 *        intro���ŗ��p����邱�Ƃ�O��Ƃ���B
 *                
 */
void nlScene_init( const SceneArray* scenes );
/**
 * @brief �V�[�����X�V���A�`��R�}���h���擾����
 */
void nlScene_Update( unsigned int nowInMs,  struct nlRenderCommands* renderCommands, nlEngineContext& cxt );
/**
 * @brief �����ŗ��p����Ă���V�[���f�[�^�ւ̎Q�Ƃ��擾����
 */
SceneArray& nlScene_getSceneData();
/**
 * @brief �����ŗ��p����Ă���v���Z�X�o�b�t�@�ւ̎Q�Ƃ��擾����
 */
ProcessBuffer& nlScene_getProcessBuffer();
/*
 * @brief �w�肵���o�ߎ���(msec�P��)�ŗL���ȃV�[���ԍ���Ԃ��B
 */
int nlScene_sceneIndex( unsigned int timeInMs );
/**
 * @brief �w�肵���o�ߎ���(msec�P��)�̃V�[����Ԃ� 
 */
nlScene& nlScene_scene( unsigned int timeInMs );
/**
 * @brief �w�肵���V�[���̊J�n�t���[����Ԃ�
 */
unsigned int nlScene_timeInMs( unsigned int sceneInde );
/**
 * @brief ���V�[������Ԃ�
 */
unsigned int nlScene_totalSceneNum();

