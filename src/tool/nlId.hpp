#pragma once

#include "main/stdafx.h"

/**
 * @brief �O���[�o����ID��o�^����
 * ����O���[�v���ł�ID�d���͋�����Ȃ��B�d�������ꍇ�̓G���[���o�͂���o�^������Ȃ��B
 */
void setId( const QString& group, const QString& tag, int id );
/**
 * @brief �O���[�o����ID��o�^����
 * ����O���[�v���ł�ID�d���͋�����Ȃ��B�d�������ꍇ�̓G���[���o�͂���o�^������Ȃ��B
 */
void setId( const QString& group, const QString& tag );

/**
 * @brief �w�肵���O���[�v���̑S�Ă�ID���N���A����
 */
void clearId( const QString& group );

/**
 * @brief ID���擾����B
 */
boost::optional<int> getId( const QString& group, const QString& tag );

/**
 * @brief ID�ɑΉ������^�O�����擾����
 */
boost::optional<QString> getTag( const QString& group, int id );

/**
 *@brief �e���̃O���[�v�S�̂��擾����
 */
typedef std::map<QString,int> Ids;
typedef std::pair<QString,int>Id;
const Ids& getIds( const QString& group );

/**
 *@brief �S�Ă�ID���擾����
 */
typedef std::map<QString,Ids> IdsGroups;
const IdsGroups& getAllIds();
