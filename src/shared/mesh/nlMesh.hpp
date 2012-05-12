#pragma once

#include "shared/nlTypedef.h"

/**
 * @brief ���b�V���̃��[�h
 * @param  const char* data      ���b�V���f�[�^
 * @param  nlUint32 dataSize ���b�V���f�[�^��
 * @param  nlMesh* model         �o�͂���郁�b�V��
 * @param  nlEngineContext* cxt  �G���W���R���e�L�X�g
 */
void nlMeshLoad( const nlInt8* data, nlUint32 dataSize, struct nlMesh* model, struct nlEngineContext* cxt );
