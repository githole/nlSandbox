#pragma once

#include "main/stdafx.h"

/**
 * @brief グローバルなIDを登録する
 * 同一グループ内でのID重複は許されない。重複した場合はエラーが出力され登録もされない。
 */
void setId( const QString& group, const QString& tag, int id );
/**
 * @brief グローバルなIDを登録する
 * 同一グループ内でのID重複は許されない。重複した場合はエラーが出力され登録もされない。
 */
void setId( const QString& group, const QString& tag );

/**
 * @brief 指定したグループ内の全てのIDをクリアする
 */
void clearId( const QString& group );

/**
 * @brief IDを取得する。
 */
boost::optional<int> getId( const QString& group, const QString& tag );

/**
 * @brief IDに対応したタグ名を取得する
 */
boost::optional<QString> getTag( const QString& group, int id );

/**
 *@brief 各当のグループ全体を取得する
 */
typedef std::map<QString,int> Ids;
typedef std::pair<QString,int>Id;
const Ids& getIds( const QString& group );

/**
 *@brief 全てのIDを取得する
 */
typedef std::map<QString,Ids> IdsGroups;
const IdsGroups& getAllIds();
