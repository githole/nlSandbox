/**/
#include "shared/nlMemory.h"
#include "shared/scene/nlScene.hpp"
#include "shared/render/nlRender.h"
#include "shared/sound/nlSound.h"
#include "shared/math/nlMath.h"
#include "shared/d3d11resource/d3d11resource.hpp"
#include "shared/mesh/nlMesh.hpp"
#include "generated/intro_data.h"

#ifdef _DEBUG
#include <cstdio>
#endif

/* use FPU */
#ifdef __cplusplus
#ifdef NDEBUG
extern "C"
{
#endif
#endif
    int _fltused=1;
    void _cdecl _check_commonlanguageruntime_version() {}
#ifdef __cplusplus
#ifdef NDEBUG
}
#endif
#endif

/**
 * @brief 起動ダイアログボックスから得られる情報
 */
struct LaunchInfo
{
    /* デモを開始するか？ */
    bool isStartDemo_;
    /**/
    /* フルスクリーン */
    bool isFullScreen_;
    /* 垂直同期 */
    bool isVSync_;
};

static LaunchInfo launchInfo;
static bool g_isExit;
static HINSTANCE g_hinst;
static nlEngineContext cxt;
static HWND resoCombo;
static LaunchInfo g_launchInfo;
static float loadingProgress;
/**/
#define ID_START       1
#define ID_QUIT        2
#define ID_FULLSCREEN  3  /*3から6の順番を変えてはいけない*/
#define ID_VSYNC       4
#define ID_RESOLUTION  5
/**/
#define EXIT_BUTTON_CAPTION "EXIT"
#define START_BUTTON_CAPTION "MAYO!"
#define dlgName "INTRO_DLG"
#define appName "INTRO"

/**/
__forceinline bool introDialog();
__forceinline void initIntro();
__forceinline void initWindow(bool isFullScreen );
__forceinline void loadAllResource();
__forceinline void initSynth();
__forceinline void mainLoop();
__forceinline void initLoading();
void updateLoading();


static const struct
{
	int width_;
	int height_;
}resolutions[]  =
{
	{1280,720},
	{1920,1080},
};

/* WinMain when intro */
void entrypoint()
{
    g_isExit = false;
    loadingProgress = 0.0f;
    /**/
    if( introDialog() )
    {
        initIntro();
        mainLoop();
    }
    ExitProcess( 0 );
}
void __cdecl main()
{
    entrypoint();
}
#ifdef _DEBUG
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow )
{
    entrypoint();
}
#endif

/**/
LRESULT CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg)  
    {
    case WM_CREATE:
        {
            cxt.rendertargets[0].width_  = resolutions[0].width_;
            cxt.rendertargets[0].height_ = resolutions[0].height_;
            /**/
            g_launchInfo.isFullScreen_ = true;
        }
        break;
    case WM_COMMAND:
        {
            const int id = LOWORD(wParam);
            if (HIWORD(wParam) == BN_CLICKED) 
            {
                switch( id )
                {
                case ID_START:
                    g_launchInfo.isStartDemo_ =true;
                    DestroyWindow(hwnd);
                    break;
                case ID_QUIT:
                    g_launchInfo.isStartDemo_ =false;
                    DestroyWindow(hwnd);
                    break;
                case ID_FULLSCREEN:
                    {
                        bool isCheaked = (IsDlgButtonChecked(hwnd, ID_FULLSCREEN) != 0);
                        CheckDlgButton(hwnd, ID_FULLSCREEN, !isCheaked );
                        g_launchInfo.isFullScreen_ = !isCheaked;
                    }
                    break;
                case ID_VSYNC:
                    {
                        bool isCheaked = (IsDlgButtonChecked(hwnd, ID_VSYNC) != 0);
                        CheckDlgButton(hwnd, ID_VSYNC, !isCheaked );
                        g_launchInfo.isVSync_ = !isCheaked;
                    }
                    break;
                }
            }
            if( HIWORD(wParam) == CBN_SELCHANGE )
            {
				const int selectIndex = SendMessage(resoCombo, CB_GETCURSEL , 0 , 0);
				cxt.rendertargets[0].width_  = resolutions[selectIndex].width_;;
				cxt.rendertargets[0].height_ = resolutions[selectIndex].height_;
            }
            break;
        }
        break;
    case WM_LBUTTONDOWN:
        ::PostMessage(hwnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return (DefWindowProc(hwnd, msg, wParam, lParam));
}
/**/
void CreateDialogBox(HWND hwnd)
{
    /**/
    HWND hDialog;
    WNDCLASSEX wc;
    nlMemset( &wc, 0, sizeof(WNDCLASSEX) );
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.lpfnWndProc      = (WNDPROC) DialogProc;
    wc.hInstance        = g_hinst;
    wc.hbrBackground    = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszClassName    = dlgName;
    RegisterClassEx(&wc);
    /**/
    RECT wndRect = {0,0,110,150};
    const DWORD dwStyleEx = WS_EX_CLIENTEDGE | WS_EX_TOPMOST;
    const DWORD dwStyle   = WS_VISIBLE | WS_POPUP | WS_DLGFRAME;
    AdjustWindowRectEx( &wndRect, dwStyle, FALSE, dwStyleEx );
    hDialog = CreateWindowEx(
        dwStyleEx,  dlgName, dlgName,dwStyle, 
        (GetSystemMetrics(SM_CXSCREEN)-wndRect.right+wndRect.left)>>1,
        (GetSystemMetrics(SM_CYSCREEN)-wndRect.bottom+wndRect.top)>>1,
        wndRect.right-wndRect.left, wndRect.bottom-wndRect.top,
        NULL, NULL, g_hinst,  NULL);
    /* 各アイテムの位置設定 */
    const int left = 5;
    const int top  = 5;
    const int comboTop  = top;
    const int comboLeft = 5;
    const int chkTop  = 40;
    const int chkDifY = 22;
    const int chkDifX = 100;
    const int btnTop    = 85;
    const int btnXDif   = 85;
    const int btnWidth  = 100;
    const int btnHeight = 30;
    resoCombo = CreateWindow(TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, comboLeft, comboTop, 100, 500, hDialog, (HMENU)ID_RESOLUTION, g_hinst, NULL);   
    CreateWindow(TEXT("button"), TEXT("FullScreen"), WS_VISIBLE | WS_CHILD | BS_CHECKBOX, left, chkTop+chkDifY*0,  90, chkDifY, hDialog, (HMENU)ID_FULLSCREEN, g_hinst, NULL);
    CreateWindow(TEXT("button"), TEXT("VSync"), WS_VISIBLE | WS_CHILD | BS_CHECKBOX,      left, chkTop+chkDifY*1,  60, chkDifY, hDialog, (HMENU)ID_VSYNC, g_hinst, NULL);
    CreateWindow(TEXT("button"), TEXT(START_BUTTON_CAPTION), WS_CHILD | WS_VISIBLE,left, btnTop, btnWidth, btnHeight, hDialog, (HMENU)ID_START, g_hinst, NULL);  
    CreateWindow(TEXT("button"), TEXT(EXIT_BUTTON_CAPTION), WS_CHILD | WS_VISIBLE, left, btnTop+30, btnWidth, btnHeight, hDialog, (HMENU)ID_QUIT, g_hinst, NULL);  
    /* NOTE: 解像度はRevision準拠のみに固定している */
    SendMessage(resoCombo, CB_ADDSTRING, 0, (LPARAM)"1280x720");
    SendMessage(resoCombo, CB_ADDSTRING, 0, (LPARAM)"1920x1080");
    SendMessage(resoCombo, CB_SETCURSEL, 0, 0);
    /**/
    CheckDlgButton(hDialog, ID_FULLSCREEN,TRUE );
    CheckDlgButton(hDialog, ID_VSYNC,TRUE );
}
/**/
LaunchInfo launchDialog()
{
    MSG msg;
    CreateDialogBox( cxt.hwnd );
    while( GetMessage(&msg, NULL, 0, 0) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage(&msg);
    }
    return g_launchInfo;
}

/* デモを開始するかを返す */
__forceinline bool introDialog()
{
    /* 一番最初にインスタンスハンドルを取得しておく */
    g_hinst = GetModuleHandle( NULL );
    /* ランチャーの起動 */
    launchInfo = launchDialog();
    return launchInfo.isStartDemo_;
}
/**/
__forceinline void initIntro()
{
    /**/
    initWindow( launchInfo.isFullScreen_ );
    /* 起動しきるのを待つ */
    Sleep(1000);
    /**/
    nlInitD3D11( cxt.hwnd, launchInfo.isFullScreen_, cxt );
    /**/
    initLoading();
    /* launchInfo.isAA_, launchInfo.isVSync_, cxt */
    nlRenderInit( &cxt );
    updateLoading();
#if 0
    /* 全てのテクスチャを生成する */
    const int numScript = g_shaderScripts.tsScripts.numScript;
    for( int i=0;i<numScript;++i)
    { 
        ntGenerateTextures(g_shaderScripts, i, &cxt ); 
        updateLoading();
    }
#endif
    /* 全てのRTを生成する */
    nlCreateRenderTarget(g_rtDescs,&cxt);
    updateLoading();
    /* 全てのシェーダを構築する */
    nlCreateShaders(ShaderType_NONE, -1, g_shaderScripts, cxt );
    updateLoading();
    /* モデルを構築する */
    for( int i=0;i<sizeof(g_models)/sizeof(*g_models);++i)
    {
        nlMeshLoad( (char*)g_models[i].data, g_models[i].size, &cxt.models[i], &cxt ); 
        updateLoading();
    }
    /* シーンを構築 */
    nlScene_init( &scenes );
    updateLoading();
    /* パスを設定する */
    NL_ASSERT( sizeof(nlCurves) == sizeof(g_64kPathes) );
    nlMemcpy( &cxt.paths, &g_64kPathes, sizeof(nlCurves) );
    updateLoading();
}
/**/
nlVertexShader loadingVS;
nlPixelShader loadingPS;
__forceinline void initLoading()
{
    /**/
    const char* commonScript = g_shaderScripts.commonScripts.script;
    const char* script = g_shaderScripts.psScripts.scripts[0].script;/* 0番目にao benchがくる事を仮定している */
    const int len = nlStrlen( commonScript ) + nlStrlen(script);
    char* shader = (char*)nlMalloc(len+1);
    nlStrcat(shader,commonScript);
    nlStrcat(shader,script);
    /**/
    loadingProgress = 0.0f;
    /* シェーダの読み込み */
    loadingVS = nlCreateVertexShader( screenVSScript, sizeof(screenVSScript), "vs", cxt );   
    loadingPS = nlCreatePixelShader( shader, nlStrlen(shader), "main", cxt );
}
/**/
void updateLoading()
{
    /**/
    ID3D11Device* d3dDevide = cxt.d3dDevice;
    ID3D11DeviceContext* d3dCxt = cxt.d3dContext;
    IDXGISwapChain* swapChain = cxt.dxgiSwpChain;
    /**/
    d3dCxt->ClearDepthStencilView( cxt.depthStencils[0].view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
    d3dCxt->OMSetRenderTargets(1, &cxt.rendertargets[0].renderTargetView_, cxt.depthStencils[0].view );
    /* ロード進捗デルタ。この定数は手動で算出すること */
    const float loadingProgressDelta = 0.00794;
    loadingProgress += loadingProgressDelta;
    nlMatrix mat;
    mat._11 = loadingProgress;
    nlCopyToD3DBuffer( cbPsMatrix[2], (char*)&mat, sizeof(nlMatrix), cxt );
    /* ロード画面を描画する */
    const float clearColor[4] = {0,0,0,0};
    cxt.d3dContext->ClearRenderTargetView( cxt.rendertargets[0].renderTargetView_, clearColor );
    cxt.d3dContext->VSSetShader( loadingVS.shader_, NULL, 0);
    cxt.d3dContext->PSSetShader( loadingPS.shader_, NULL, 0);
    cxt.d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
    cxt.d3dContext->Draw( 4, 0 );
    /* 画面更新 */
    d3dCxt->Flush();
    NL_HR_VALID( swapChain->Present( 1, 0) );
    /**/
    Sleep(100); /* AOベンチをじっくり見せたい場合はここをコメントアウト */
    /* Flipを繰り返すと途中でウィンドウが固まってしまうので、メッセージポンプしておく */
    MSG msg;
    while( PeekMessage( &msg,NULL,0,0,PM_REMOVE ) && (msg.message != WM_QUIT) )
    { 
        TranslateMessage( &msg ); 
        DispatchMessage( &msg );
    }
}
/**/
static LRESULT CALLBACK mainWindowProc(
                                       HWND hWnd, 
                                       UINT msg, 
                                       WPARAM wParam, 
                                       LPARAM lParam)
{
    switch (msg) 
    {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        ::PostMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam);
        break;
    default:
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}
/**/
void initWindow( bool isFullScreen )
{
    const unsigned int XRES = cxt.rendertargets[0].width_; /* @FIXME クライアント領域などを考慮する */
    const unsigned int YRES = cxt.rendertargets[0].height_;
    DEVMODE screenSettings = { 
        {0},
        0,0,156,0,0x001c0000,{0},0,0,0,0,0,{0},0,32,XRES,YRES,{0}, 0,
        0,0,0,0,0,0,
        0,0
    };

    DWORD			dwExStyle, dwStyle;
    RECT			rec;
    WNDCLASSA		wc;

    nlMemset( &wc, 0, sizeof(WNDCLASSA) );
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = mainWindowProc;
    wc.hInstance     = g_hinst;
    wc.lpszClassName = appName;

    if( !RegisterClass((WNDCLASSA*)&wc) )
        return;

    if( isFullScreen )
    {
#if 0
        if( ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return;
#endif    
		dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        ShowCursor( 0 );
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_CAPTION;
    }
    rec.left   = 0;
    rec.top    = 0;
    rec.right  = XRES;
    rec.bottom = YRES;

    if( isFullScreen )
    {
        cxt.hwnd = CreateWindowEx( 
            dwExStyle, 
            wc.lpszClassName, 
            wc.lpszClassName, 
            dwStyle, 0, 0, 
            rec.right-rec.left, rec.bottom-rec.top, 0, 0, g_hinst, 0 );
    }
    else
    {
        AdjustWindowRect( &rec, dwStyle, 0 );
        cxt.hwnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
            (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
            (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
            rec.right-rec.left, rec.bottom-rec.top, 0, 0, g_hinst, 0 );
    }
    ShowWindow( cxt.hwnd, SW_SHOW );
    UpdateWindow(cxt.hwnd);
}
/* main loop */
__forceinline void mainLoop()
{
    size_t sceneStartTime = 0;
    MSG msg;
    int actualFrame = 0;

    /* 音楽をロードし再生する */
    nlSound* sound = 0;
    sound = nlSound_constructFromMemory( (char*)soundData, sizeof(soundData)/sizeof(*soundData), &cxt );
    nlSound_play(sound,0);
    /**/
    unsigned int start = timeGetTime();
    /**/
	unsigned int totalRrenderCount = 0;
	/**/
    while ( GetAsyncKeyState( VK_ESCAPE ) == 0 && !g_isExit  )
    {
        if ( PeekMessage ( &msg,NULL,0,0,PM_REMOVE ) )
        {
            if(msg.message == WM_QUIT) break;
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            /* 現在のフレームの算出 */
            const int elapseMs  = timeGetTime()-start;
            /* フレーム数が全シーンの終端を越えていたら終了する */
            const int finalMs = ((3*60)+14)*1000; /* 3分18秒。これは曲によって決め打ちにする。 */
            if(finalMs<elapseMs)
            { break; }
            /* update scene */
            nlRenderCommands renderCommands;
            renderCommands.clear();
            nlScene_Update( elapseMs, &renderCommands, cxt );
#if 0
            /* デバッグ出力 */
            printf("scene:%2d frame:%5d ",nlScene_sceneIndex(curFrame),curFrame );
            //printf("RCMD:%2d", renderCommands.commandIndex_ );
            const nlScene& scene = nlScene_scene(curFrame);
            printf("sceneNodeNum:%2d sceneFrame:%4d ",scene.numSceneMap_, scene.sceneFrame );
            puts("");
#endif
            /* render scene */
            nlRender(renderCommands,&cxt);
			++totalRrenderCount;
        }
    }
	char buf[0xff];
	wsprintf( buf, "totalFrame:%d\n", totalRrenderCount );
	OutputDebugString( buf );
	//totalRrenderCount;
}
