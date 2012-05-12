#ifndef _STD_AFX_H_
#define _STD_AFX_H_

#pragma warning(disable:4244) /* 数値の変換 */
#pragma warning(disable:4100) /* 参照されない引数 */
#pragma warning(disable:4305) /* 数値の切り詰め */
#pragma warning(disable:4189) /* 参照されないローカル変数 */
#pragma warning(disable:4389) /**/
#pragma warning(disable:4101) /**/
#pragma warning(disable:4800) /**/
#pragma warning(disable:4245) /**/
#pragma warning(disable:4996) /* 推奨されない関数 */

#ifndef INTROMODE
    /* CRT */
    #include <string>
    #include <float.h>
    #include <math.h>
    #include <shlwapi.h>
    #include <limits.h>
	#include <iostream>
    #pragma comment(lib,"shlwapi.lib")
    /* STL */
    #include <vector>
    #include <set>
    #include <map>
    #include <list>
    #include <deque>
    #include <sstream>
    /* windows */
    #include <comip.h>
    /* Boost */
    #include <boost/scoped_ptr.hpp> 
    #include <boost/shared_ptr.hpp>
    #include <boost/variant.hpp>
    #include <fstream>
    #include <boost/archive/text_oarchive.hpp>
    #include <boost/archive/text_iarchive.hpp>
    #include <boost/serialization/vector.hpp>
    #include <boost/serialization/map.hpp>
    #include <boost/serialization/string.hpp>
    #include <boost/serialization/serialization.hpp>
    #include <boost/serialization/nvp.hpp>
    #include <boost/archive/xml_oarchive.hpp>
    #include <boost/archive/xml_iarchive.hpp>
    #include <boost/foreach.hpp>
    #include <boost/unordered_map.hpp>
    #include <boost/filesystem/path.hpp>
    #include <boost/filesystem/operations.hpp>
    #include <boost/filesystem/fstream.hpp>
	#include <boost/lexical_cast.hpp>
	#include <boost/algorithm/string.hpp>
    #include <boost/preprocessor.hpp>

#pragma warning(push)
#pragma warning(disable:4345) /* warning C4345: 動作変更 : 形式 () の初期化子で構築される POD 型のオブジェクトは既定初期化されます。	*/
	#include <boost/make_shared.hpp>
#pragma warning(pop)
    #include <boost/optional.hpp>

    /**/
#pragma warning(push)
#pragma warning(disable:4127) /* 条件式が定数 */
#pragma warning(disable:4512) /* 代入演算子が生成できない */
    #include <QtGui>
#pragma warning(pop)

    /**/
    #include <atlbase.h>
    #include <atlconv.h>
#endif

/* Windows */
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

/* directX */
#include <d3d10_1.h>
#include <D3DX10Async.h>
#pragma comment(lib,"d3d10.lib")
#include <d3d11.h>
#include <d3dx11.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#include <d3dx9math.h>
#include <dxerr.h>
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")


#ifndef INTROMODE
    #include <crtdbg.h>
    #ifdef _DEBUG
    #define   new                   new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define   malloc(s)             _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #define   calloc(c, s)          _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #define   realloc(p, s)         _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #define   _recalloc(p, c, s)    _recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #define   _expand(p, s)         _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #endif
#endif

#endif //_STD_AFX_H_
