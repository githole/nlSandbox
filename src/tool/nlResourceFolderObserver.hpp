#pragma once

#include "main/stdafx.h"

/**
* @brief 
*/
class nlResourceFolderObserver
    :public QObject
{
    Q_OBJECT
protected:
    /**/
    nlResourceFolderObserver( struct nlEngineContext* cxt );
    /**/
    void initialize(QString dir, QString ext );
    /**/
    ~nlResourceFolderObserver();
public:
    const QFileInfoList& fileList()
    { return fileList_; }
signals:
    void onChangeContent();
private slots:
    void cheackAllFile();
protected:
    virtual void onLoad( const QFileInfo& path ) = 0;

private:
    QTimer timer_;
    QFileInfoList fileList_;
protected:
    struct nlEngineContext* cxt_;
};

#if 0
/** 
* @brief テクスチャフォルダ変更の監視
*/
class TSFolderObserver
    :public nlResourceFolderObserver
{
public:
    TSFolderObserver(struct nlEngineContext* cxt);
    ~TSFolderObserver();
    void onLoad( const QFileInfo& path );
    const QString shaderFilePath()const;
    std::vector<QByteArray> funcList()const;
private:
    QString groupName_;
};
#endif

/** 
* @brief メッシュフォルダ変更の監視
*/
class MeshFolderObserver
    :public nlResourceFolderObserver
{
public:
    MeshFolderObserver(nlEngineContext* cxt);
    ~MeshFolderObserver();
    void onLoad( const QFileInfo& path );
private:
};
/** 
* @brief Musicフォルダ変更の監視
*/
class MusicFolderObserver
    :public nlResourceFolderObserver
{
public:
    MusicFolderObserver(struct nlEngineContext* cxt);
    ~MusicFolderObserver();
    void onLoad( const QFileInfo& path );
};
/** 
* @brief rendertargetフォルダ変更の監視
*/
class RTFolderObserver
    :public nlResourceFolderObserver
{
public:
    RTFolderObserver(nlEngineContext* cxt);
    ~RTFolderObserver();
    void onLoad( const QFileInfo& path );
private:

};
/** 
* @brief compute shaderフォルダ変更の監視
*/
class CSFolderObserver
    :public nlResourceFolderObserver
{
public:
    CSFolderObserver(nlEngineContext* cxt);
    ~CSFolderObserver();
    void onLoad( const QFileInfo& path );
};
/** 
* @brief geometory shaderフォルダ変更の監視
*/
class GSFolderObserver
    :public nlResourceFolderObserver
{
public:
    GSFolderObserver(nlEngineContext* cxt);
    ~GSFolderObserver();
    void onLoad( const QFileInfo& path );
private:
    QString groupName_;
};
/** 
* @brief pixel shaderフォルダ変更の監視
*/
class PSFolderObserver
    :public nlResourceFolderObserver
{
public:
    PSFolderObserver(
        struct nlEngineContext* cxt );
    ~PSFolderObserver();
    void onLoad( const QFileInfo& path );
private:
    QString groupName_;
};
/** 
* @brief vertex shaderフォルダ変更の監視
*/
class VSFolderObserver
    :public nlResourceFolderObserver
{
public:
    VSFolderObserver(struct nlEngineContext* cxt);
    ~VSFolderObserver();
    void onLoad( const QFileInfo& path );
private:
    QString groupName_;
};
/** 
* @brief shader共有ファイル変更の監視
*/
class CommonShaderObserver
    :public nlResourceFolderObserver
{
public:
    CommonShaderObserver(struct nlEngineContext* cxt);
    ~CommonShaderObserver();
    void onLoad( const QFileInfo& path );
private:
};
