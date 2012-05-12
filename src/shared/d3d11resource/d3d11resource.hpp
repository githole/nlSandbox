#pragma once

#include "shared/nlTypedef.h"

/**
 * @brief
 */
void nlInitD3D11( HWND hWnd, bool isFullScreen, struct nlEngineContext& cxt );
/**
 * @brief
 */
void nlUpdateViewPortSetting(struct nlEngineContext& cxt);
/**
 * @brief
 */
void nlUpdateRasterizeSetting(struct nlEngineContext& cxt);
/**
 * @brief
 */
void nlUpdateDepthStencilSetting(struct nlEngineContext& cxt);
/**
 * @brief
 */
void nlUpdateSamplerState(struct nlEngineContext& cxt);
/**
 * @brief
 */
ID3D11Buffer* nlCreateVertexBuffer( unsigned int size, nlEngineContext& cxt );
/**
 * @brief
 */
ID3D11Buffer* nlCreateIndexBuffer( unsigned int size, nlEngineContext& cxt );
/**
 * @brief
 */
ID3D11Buffer* nlCreateConstBuffer( unsigned int size, nlEngineContext& cxt );
/**
 * @brief
 */
void nlCreateRenderTarget( const RenderTargetDescs& descs, nlEngineContext* cxt );
/**
 * @brief
 */
nlVertexShader nlCreateVertexShader( const nlInt8* script, unsigned int scriptSize, const nlInt8* funcName, nlEngineContext& cxt );
/**
 * @brief
 */
nlGeometoryShader nlCreateGeometoryShader( const nlInt8* script, unsigned int scriptSize, const nlInt8* funcName, nlEngineContext& cxt );
/**
 * @brief
 */
nlPixelShader nlCreatePixelShader( const nlInt8* script, unsigned int scriptSize, const nlInt8* funcName, nlEngineContext& cxt );
/**
 * @brief
 */
void nlCopyToD3DBuffer( ID3D11Buffer* constBuffer, const nlInt8* data, unsigned int dataSize, nlEngineContext& cxt );
/**
 * @brief �V�F�[�_�̃��[�h���s��
 * @param ShaderType shaderType  �V�F�[�_�̎�ށBShaderType_NONE���w�肷��ƑS�ẴV�F�[�_�����[�h�����B
 * @param int targetIndex        �^�[�Q�b�g�ɂȂ�V�F�[�_�̃C���f�b�N�X�B-1���w�肷��ƑS�ẴC���f�b�N�X�����[�h�����B
 * @param const ShaderScripts& shaderScript, �V�F�[�_�̃X�N���v�g
 * @param nlEngineContext& cxt   �R���e�L�X�g
 */
void nlCreateShaders( ShaderType shaderType, int targetIndex, const ShaderScripts& shaderScript, nlEngineContext& cxt );

#if 0
/**
 * @brief �e�N�X�`���̐���
 * @param const ShaderScripts& shaderScript �V�F�[�_�̃X�N���v�g
 * @param unsigned int index �ΏۂɂȂ�e�N�X�`���ԍ�
 * @param nlEngineContext* cxt �R���e�L�X�g
 */
void ntGenerateTextures(const ShaderScripts& shaderScript, unsigned int index, nlEngineContext* cxt );
#endif
