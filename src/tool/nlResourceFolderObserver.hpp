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
* @brief �e�N�X�`���t�H���_�ύX�̊Ď�
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
* @brief ���b�V���t�H���_�ύX�̊Ď�
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
* @brief Music�t�H���_�ύX�̊Ď�
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
* @brief rendertarget�t�H���_�ύX�̊Ď�
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
* @brief compute shader�t�H���_�ύX�̊Ď�
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
* @brief geometory shader�t�H���_�ύX�̊Ď�
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
* @brief pixel shader�t�H���_�ύX�̊Ď�
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
* @brief vertex shader�t�H���_�ύX�̊Ď�
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
* @brief shader���L�t�@�C���ύX�̊Ď�
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
