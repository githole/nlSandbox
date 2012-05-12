#include "stdafx.h"
#include "shared/texture/nltexgen.hpp"
#include "tool/nlTExgenEx.hpp"
#include "shared/math/nlMath.h"

using namespace std;

/**/
const char* nlItoa( unsigned int value )
{
    static char buf[16];
    unsigned int keta = (unsigned int)nlLog10( value )+1;
    unsigned int base = 1;
    for( unsigned int i=0;i<keta;++i)
    {        
        buf[keta-i-1] = ((value/base) % 10) + '0';
        base *= 10;
    }
    buf[keta] = '\0';
    return buf;
}

void readFiles( string& masterScript, 
               const char* funcNames[MAX_TEXTURE_NUM],
               unsigned int* numTexture )
{
    namespace fs = boost::filesystem;
    /**/
    TCHAR path[ MAX_PATH ];
    GetModuleFileName( NULL, path, sizeof(path) );
    PathRemoveFileSpec( path );
    wcscat( path, L"\\TexShader\\" );
    SetCurrentDirectory( path );
    /**/
    int numScript = 0;
    std::vector<std::string> paths;
    /* ファイルのリストを取得する */
    fs::directory_iterator end;
    for( fs::directory_iterator it(fs::current_path()); it!=end; ++it )
    {
        fs::path p = it->path();
        /* 拡張子がtsのものだけを利用する */
        if( p.extension() == ".ts" )
        {
            paths.push_back(p.string());
            /**/
            char* newName = (char*)calloc( strlen(it->leaf().c_str())+1, 1 );
            strcpy( newName, p.stem().c_str() );
            funcNames[numScript] = newName;
            ++numScript;
        }   
    }
    *numTexture = numScript;

    /**/
    const char* header =
    {
        "SamplerState smp\n"
        "{\n"
        "Filter=MIN_MAG_MIP_LINEAR;\n"
        "AddressU=Clamp;\n"
        "AddressV=Clamp;\n"
        "};\n"
        "struct VO\n"
        "{\n"
        "float4 pos:SV_POSITION;"
        "float2 uv:TEXCOORD0;\n"
        "};\n"
        "VO vs(\n"
        "uint vid : SV_VertexID )\n"
        "{\n"
        "VO ret;\n"
        "if (vid == 0)     {ret.pos = float4(-1, -1, 0, 1);ret.uv=float2(0,0);}\n"
        "else if (vid == 1){ret.pos = float4( 1, -1, 0, 1);ret.uv=float2(1,0);}\n"
        "else if (vid == 2){ret.pos = float4(-1,  1, 0, 1);ret.uv=float2(0,1);}\n"
        "else              {ret.pos = float4( 1,  1, 0, 1);ret.uv=float2(1,1);}\n"
        "return ret;\n"
        "}\n"
    };
    /**/
    masterScript.append( header );
    for(int i=0;i<numScript;++i)
    {
        masterScript.append("Texture2D tex");
        masterScript.append(funcNames[i]);
        masterScript.append(":register(t" );        
        masterScript.append(nlItoa(i));
        masterScript.append(");\n");
    }
    /* 本体を追加 */
    for( int i=0;i<numScript;++i)
    {
        /* ファイルの内容を取得 */
        std::string path = paths[i];
        FILE* fp = fopen( path.c_str(), "rt" );
        fseek( fp, 0, SEEK_END );
        size_t size = ftell(fp);
        fseek( fp, 0, SEEK_SET );

        unsigned int hoge = strlen(masterScript.c_str());
        int startPos = masterScript.size();
        masterScript.resize(masterScript.size()+size);
        fread( &masterScript[startPos], size, 1, fp  );
        fclose(fp);
        /**/
        masterScript.resize(strlen(masterScript.c_str()));
    }

#if 1
    /* シェーダ全体をテストで出力する */
    FILE* fp = fopen("./tmp.txt", "wt");
    fwrite( masterScript.c_str(), masterScript.size(), 1, fp );
    fclose(fp);
#endif
}
/**/
int main(int argc, const char* argv[]  )
{
    /**/
    puts("start read file");
    nlTexture2 texuters[MAX_TEXTURE_NUM] = {};
    /* マスターシェーダの作成 */
    string masterScript;
    const char* funcNames[MAX_TEXTURE_NUM];
    unsigned int numTexture = 0;
    readFiles(masterScript, funcNames, &numTexture );

    /**/
    ID3D11Device*        d3dDevice;
    ID3D11DeviceContext* d3dContext;
    D3D_FEATURE_LEVEL fl;
    HWND hwnd = CreateWindowExA(0,"EDIT",0,0,0,0,512,512,0,0,GetModuleHandleA(NULL),0);
    D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE,(HMODULE)0,0,NULL,0, D3D11_SDK_VERSION, &d3dDevice, &fl, &d3dContext );
    /**/
    puts("start generate");
    ntGenerateTextures( d3dDevice, d3dContext, masterScript.c_str(), funcNames, texuters, numTexture );
    /**/
    puts("start write ppm");
    /* 全てのファイルを保存する */
    for( int i=0;i<numTexture;++i)
    {
        /**/
        char fileName[MAX_PATH] = {'\0'};
        sprintf( fileName, "./%s.ppm", funcNames[i] );
        saveToPPM( fileName, texuters[i].texture, d3dDevice, d3dContext );
    }
}
