#pragma once

#include "shared/nlTypedef.h"

/**
 * @brief メッシュのロード
 * @param  const char* data      メッシュデータ
 * @param  nlUint32 dataSize メッシュデータ長
 * @param  nlMesh* model         出力されるメッシュ
 * @param  nlEngineContext* cxt  エンジンコンテキスト
 */
void nlMeshLoad( const nlInt8* data, nlUint32 dataSize, struct nlMesh* model, struct nlEngineContext* cxt );
