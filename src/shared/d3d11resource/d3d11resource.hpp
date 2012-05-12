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
 * @brief シェーダのロードを行う
 * @param ShaderType shaderType  シェーダの種類。ShaderType_NONEを指定すると全てのシェーダがロードされる。
 * @param int targetIndex        ターゲットになるシェーダのインデックス。-1を指定すると全てのインデックスがロードされる。
 * @param const ShaderScripts& shaderScript, シェーダのスクリプト
 * @param nlEngineContext& cxt   コンテキスト
 */
void nlCreateShaders( ShaderType shaderType, int targetIndex, const ShaderScripts& shaderScript, nlEngineContext& cxt );

#if 0
/**
 * @brief テクスチャの生成
 * @param const ShaderScripts& shaderScript シェーダのスクリプト
 * @param unsigned int index 対象になるテクスチャ番号
 * @param nlEngineContext* cxt コンテキスト
 */
void ntGenerateTextures(const ShaderScripts& shaderScript, unsigned int index, nlEngineContext* cxt );
#endif
