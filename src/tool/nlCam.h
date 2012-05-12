#pragma once
#include "main/stdafx.h"
#include "shared/math/nlMath.h"
#include "shared/nlEngineContext.hpp"

/**
 * @brief カメラ位置
 * @note  旧バージョンとの互換性のために残されていたが、近いうちに破棄される。
 */
struct nlCamDesc
{
    nlVec3 pos_;
    nlVec3 lookat_;
    nlVec3 updir_;
    float  fovy_;
    float  aspect_;
    float  zn_;
    float  zf_;
};

class nlCamera
{
public:
    virtual void update(nlEngineContext& cxt ) = 0;
    virtual void onRButtonClick(int mx, int my) = 0;
    virtual void onMButtonClick(int mx, int my) = 0;
    virtual void onRButtonDrag(int mx, int my) = 0;
    virtual void onMButtonDrag(int mx, int my) = 0;
    virtual void onMouseWheel(int delta) = 0;
};

class nlCamFreedom
    :public nlCamera
{
    float getFloat( QSettings& setting, const char* tagStr, float defaultVal )
    {
        QString tag(tagStr);
        QVariant tmp = setting.value(tag);
        if(tmp.isNull())
        {
            return defaultVal;
        }
        else
        {
            return tmp.toFloat();
        }
    }
public:
    nlCamFreedom()
    {
        prevMousePoint_.x = 0;
        prevMousePoint_.y = 0;

        eyePos_ = nlVec3_create(10.0f,10.0f,10.0f);
        lookAt_ = nlVec3_create(0.0f,0.0f,0.0f);
        upDir_  = nlVec3_create( 0.0f,1.0f,0.0f);
        camTheta_ = 0.0f;
        camFai_ = 0.0f;
        eyeLookAtDist_ = 2.0f;
        eyeDir_ = nlVec3_create(1.0f,10.0f,10.0f);
        curPose_ = nlQuaternion::create( -0.65180898, 0.0011354019, 1.1264923, 0.71188200 );
    }
    /**/
    ~nlCamFreedom()
    {
        ///* カメラの設定を保存する */
        //QSettings setting( getIniPath() );
        //setting.setValue("pose_x", curPose_.x_ );
        //setting.setValue("pose_y", curPose_.y_ );
        //setting.setValue("pose_z", curPose_.z_ );
        //setting.setValue("pose_w", curPose_.w_ );
        //setting.setValue("distance", eyeLookAtDist_ );
        //setting.setValue("lookAt_x", lookAt_.x_ );
        //setting.setValue("lookAt_y", lookAt_.y_ );
        //setting.setValue("lookAt_z", lookAt_.z_ );
    }
    /**/
    void update( nlEngineContext& cxt )
    {
        /**/
        const float aspect = (float)cxt.rendertargets[0].width_/(float)cxt.rendertargets[0].height_;
        const nlVec3 upDirDefault      = nlVec3_create(0.0f,1.0f,0.0f);
        const nlVec3 aspectFovYDefault = nlVec3_create(aspect, D3DX_PI/2.0f, 0.0f );//TODO ちゃんとした値にする
        const nlVec3 nzfzDefault       = nlVec3_create(0.05f, 1000.0f, 0.0f);//TODO ちゃんとした値にする

        aspectFovy_ = aspectFovYDefault;
        nzfz_       = nzfzDefault;
        nlVec3 origEyeDir = nlVec3_create(0.0f,0.0f,1.0f);
        nlVec3 origUp     = nlVec3_create(0.0f,1.0f,0.0f);
        rotateVec3ByQuaternion( &eyeDir_, &origEyeDir, (D3DXQUATERNION*)&curPose_ );
        rotateVec3ByQuaternion( &upDir_,  &origUp,     (D3DXQUATERNION*)&curPose_ );
        eyePos_.x_ = eyeDir_.x_ * eyeLookAtDist_ + lookAt_.x_;
        eyePos_.y_ = eyeDir_.y_ * eyeLookAtDist_ + lookAt_.y_;
        eyePos_.z_ = eyeDir_.z_ * eyeLookAtDist_ + lookAt_.z_; 

        /* 全てコピーする */
        /*g_cameDesc.aspect_ = aspectFovy_.x_;
        g_cameDesc.fovy_   = aspectFovy_.y_;
        g_cameDesc.lookat_ = lookAt_;
        g_cameDesc.pos_    = eyePos_;
        g_cameDesc.updir_  = upDir_;
        g_cameDesc.zn_     = nzfz_.x_;
        g_cameDesc.zf_     = nzfz_.y_;*/
    }
    /**/
    void onRButtonClick(int mx, int my)
    {
        prevMousePoint_.x = mx;
        prevMousePoint_.y = my;
    }
    /**/
    void onMButtonClick(int mx, int my)
    {
        prevMousePoint_.x = mx;
        prevMousePoint_.y = my;
    }
    /**/
    void onRButtonDrag(int mx, int my)
    {
        const int dX = prevMousePoint_.x - mx;
        const int dY = prevMousePoint_.y - my;
        const float upAngle = dY/100.0f;
        const float rightAngle = dX/100.0f;
        D3DXQUATERNION qtnRot;
        D3DXQuaternionRotationYawPitchRoll( &qtnRot, rightAngle, upAngle, 0.0f );
        D3DXQuaternionMultiply( (D3DXQUATERNION*)&curPose_, (D3DXQUATERNION*)&qtnRot, (D3DXQUATERNION*)&curPose_ );
        prevMousePoint_.x = mx;
        prevMousePoint_.y = my;
    }
    /**/
    void onMButtonDrag(int mx, int my)
    {
        const float dX = -(prevMousePoint_.x - mx)/100.0f;
        const float dY = (prevMousePoint_.y - my)/100.0f;
        const nlVec3& e = eyePos_;
        const nlVec3& u = upDir_;
        nlVec3 rightDir;
        D3DXVec3Cross( (D3DXVECTOR3*)&rightDir, (D3DXVECTOR3*)&eyeDir_, (D3DXVECTOR3*)&u );
        const float mdx = rightDir.x_*dX - u.x_*dY;
        const float mdy = rightDir.y_*dX - u.y_*dY;
        const float mdz = rightDir.z_*dX - u.z_*dY;
        nlVec3& l = lookAt_;
        float factor = eyeLookAtDist_ * 0.3f;
        l.x_ += mdx*factor;
        l.y_ += mdy*factor;
        l.z_ += mdz*factor;
        prevMousePoint_.x = mx;
        prevMousePoint_.y = my;
    }
    /**/
    void onMouseWheel(int delta)
    {
        eyeLookAtDist_ *= (1.0f - delta/1000.0f);
    }
    /**/
    nlCamDesc camDesc() const
    {
        nlCamDesc ret;
        ret.aspect_ = aspectFovy_.x_;
        ret.fovy_   = aspectFovy_.y_;
        ret.lookat_ = lookAt_;
        ret.pos_    = eyePos_;
        ret.updir_  = upDir_;
        ret.zf_     = nzfz_.y_;
        ret.zn_     = nzfz_.x_;
        return ret;
    }
    /* 上方向だけを上向きにする */
    void upDirCorrect()
    {

    }
    /* 視点を最初の状態にする */
    void defautlView()
    {
        const float a = -D3DX_PI/4.0f;
        const float b =  D3DX_PI/2.0f;
        const float c =  D3DX_PI/4.0f;
        curPose_.x_ = sin(a);
        curPose_.y_ = sin(b);
        curPose_.z_ = sin(c);
        curPose_.w_ = cos(a)+cos(b)+cos(c);

        eyeLookAtDist_ = 2.0f;
        lookAt_.x_ = 0.0f;
        lookAt_.y_ = 0.0f;
        lookAt_.z_ = 0.0f;
    }
public:
    nlVec3   eyePos_;
    nlVec3   lookAt_;
    nlVec3   upDir_;
    nlVec3   aspectFovy_;
    nlVec3   nzfz_;
private:
    float camTheta_;
    float camFai_;
    float eyeLookAtDist_;
    nlVec3 eyeDir_;
    nlQuaternion curPose_;
    POINT prevMousePoint_;
};