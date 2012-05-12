#include "main/stdafx.h"
#include "SceneEditor.h"
#include "tool/nlId.hpp"

/**/
SceneEditor::SceneEditor( QWidget* widget )
    :NodeEditor( widget )
{
}
/**/
SceneEditor::~SceneEditor()
{
}
/**/
void SceneEditor::onAddNode()
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onConnectNode()
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onDisconnectNode()
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onRemoveNode()
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onForcusNode()
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onDisforcusNode()
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onClick( boost::optional<int> clickedNode )
{
    /* TODO é¿ëï */
}
/**/
void SceneEditor::onDoubleClick( boost::optional<int> clickedNode )
{
    /* TODO é¿ëï */
}
/**/
NodeeditorboardDesc SceneEditor::nodeTypeList()const
{
    NodeeditorboardDesc desc;
    { /* Nop */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Nop";
        nodeFuture.nodeId_ = SNT_NOP;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* MatNop */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "MatNop";
        nodeFuture.nodeId_ = SNT_MAT_NOP;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src", NL_ET_Matrix ) );
        nodeFuture.nodeType_ = NodeType_Util;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Add */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Add";
        nodeFuture.nodeId_ = SNT_ADD;
        nodeFuture.description_ = "r=e1+e2";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src1") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src2") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Sub */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Sub";
        nodeFuture.nodeId_ = SNT_SUB;
        nodeFuture.description_ = "r=e1-e2";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src1") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src2") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Div */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Div";
        nodeFuture.nodeId_ = SNT_DIV;
        nodeFuture.description_ = "r=e1/e2";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("div") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Mul */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Mul";
        nodeFuture.nodeId_ = SNT_MUL;
        nodeFuture.description_ = "r=e1*e2";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src1") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src2") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    {
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "MinMax";
        nodeFuture.nodeId_ = SNT_MINMAX;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("v1") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("v2") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "isMax" , VPT_INT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

    { /* Dif */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Dif";
        nodeFuture.nodeId_ = SNT_DIF;
        nodeFuture.description_ = "e[i+offset]-e[i]";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "offset" , VPT_INT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Rot2 */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Rot2";
        nodeFuture.nodeId_ = SNT_ROT2;
        nodeFuture.description_ = "rot dir";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("ope") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Trance */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Trance";
        nodeFuture.nodeId_ = SNT_TRANCE;
        nodeFuture.description_ = "add xyz";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "x", VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "y", VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "z", VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Trance2 */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Trance2";
        nodeFuture.nodeId_ = SNT_TRANCE2;
        nodeFuture.description_ = "add xyzw";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "x", VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "y", VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "z", VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "w", VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

    { /* Scale */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Scale";
        nodeFuture.nodeId_ = SNT_SCALE;
        nodeFuture.description_ = "scale all element";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "scale" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

#if 0
    { /* Clamp */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Clamp";
        nodeFuture.nodeId_ = SNT_CLAMP;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "lower" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "upper" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
#endif

    { /* sin */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Sin";
        nodeFuture.nodeId_ = SNT_SIN;
        nodeFuture.description_ = "r=sin(e)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "radius" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

    { /* cos */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Cos";
        nodeFuture.nodeId_ = SNT_COS;
        nodeFuture.description_ = "r=cos(e)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "radius" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* CosSin */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "CosSin";
        nodeFuture.nodeId_ = SNT_COSSIN;
        nodeFuture.description_ = "x=cos(ix) y=sin(iy)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "radius" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Atan */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Atan";
        nodeFuture.nodeId_ = SNT_ATAN;
        nodeFuture.description_ = "x=atan(x)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Norm */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Norm";
        nodeFuture.nodeId_ = SNT_NORM;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "scale" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* tan */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Tan";
        nodeFuture.nodeId_ = SNT_TAN;
        nodeFuture.description_ = "r=tan(e)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "radius" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

    { /* exp */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Exp";
        nodeFuture.nodeId_ = SNT_EXP;
        nodeFuture.description_ = "r=b*exp(a*x)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "a" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "b" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* expstep */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "ExpStep";
        nodeFuture.nodeId_ = SNT_EXPSTEP;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "k" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "n" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* gannma */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Gamma";
        nodeFuture.nodeId_ = SNT_GAMMA;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "begin" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "end" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "gannma" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

    { /* mod */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Mod";
        nodeFuture.nodeId_ = SNT_MOD;
        nodeFuture.description_ = "r=fmod(e,t)";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "cutVale" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

    { /* length */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Length";
        nodeFuture.nodeId_ = SNT_LENGTH;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Distance */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Distance";
        nodeFuture.nodeId_ = SNT_DISTANCE;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("from") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "x" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "y" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "z" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "w" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* repeat */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Repeat";
        nodeFuture.nodeId_ = SNT_REPEAT;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src") );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

#if 0
    { /* tan */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "SmoothStep";
        nodeFuture.nodeId_ = SNT_SMOOTHSTEP;
        nodeFuture.description_ = "smooth step for x";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "start" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "end" ,   VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Math;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
#endif

    { /* ConstV4 */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "ConstV4";
        nodeFuture.nodeId_ = SNT_CONSTV4;
        nodeFuture.description_ = "r=Vec4(x,y,z,w)";
        nodeFuture.props_.push_back( NodeFeature::PropProp( "x" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "y" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "z" , VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "w" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* MakeV4 */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "MakeV4";
        nodeFuture.nodeId_ = SNT_MAKEV4;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("x") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("y") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("z") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("w") );
        nodeFuture.description_ = "";
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Swizzle2 */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Swizzle2";
        nodeFuture.nodeId_ = SNT_SWIZZLE2;
        nodeFuture.description_ = "Swizzle Vec4Array";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "xIn" , VPT_INT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "yIn" , VPT_INT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "zIn" , VPT_INT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "wIn" , VPT_INT ) );
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

#if 0
    { /* Binarize */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Binarize";
        nodeFuture.nodeId_ = SNT_BINARIZE;
        nodeFuture.description_ = "Binarize!";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "threthold" , VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
#endif

#if 0
    { /* Blend */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Blend";
        nodeFuture.nodeId_ = SNT_BLEND;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input1") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input2") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("factor") );
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
#endif

    { /* SceneTime */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Time";
        nodeFuture.nodeId_ = SNT_SCENETIME;
        nodeFuture.description_ = "SceneTime";
        nodeFuture.props_.push_back( NodeFeature::PropProp( "scale", VPT_FLOAT ) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "offset", VPT_FLOAT ) );
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* SystemConst */
        const char* groupName = "SystemConst";
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "SystemConst";
        nodeFuture.nodeId_ = SNT_SYSTEM_CONST;
        nodeFuture.description_ = "";
        nodeFuture.props_.push_back( NodeFeature::PropProp( groupName, VPT_STRING_TO_ID ) );
        nodeFuture.nodeType_ = NodeType_Util;
        nodeFuture.outEdgeType_ = NL_ET_Vec4;
        desc.nodeFeatures_.push_back( nodeFuture );
        /**/
        setId( groupName, "Resolution", 0 );
        setId( groupName, "Aspect", 1 );
        //setId( groupName, "Resolution", 0 );
    }
    { /* range */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Range3D";
        nodeFuture.nodeId_ = SNT_RANGE;
        nodeFuture.description_ = "range";
        nodeFuture.props_.push_back( NodeFeature::PropProp( "loopX" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "loopY" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "difX" , VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "difY" , VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "difZ" , VPT_FLOAT ));
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* subarray */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "SubArray";
        nodeFuture.nodeId_ = SNT_SUBARRAY;
        nodeFuture.description_ = "range";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "start" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }

#if 0
    { /* adressing */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Adressing";
        nodeFuture.nodeId_ = SNT_ADRESS;
        nodeFuture.description_ = "0:clamp 1:wrap";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "start" , VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "end" , VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "mode" , VPT_INT));
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
#endif

    { /* random */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Random";
        nodeFuture.nodeId_ = SNT_RANDOM;
        nodeFuture.description_ = "rand(min,max)";
        nodeFuture.props_.push_back( NodeFeature::PropProp( "seed" , VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "min" , VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "max" , VPT_FLOAT ));
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Curve */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Curve";
        nodeFuture.nodeId_ = SNT_CURVE;
        nodeFuture.description_ = "get curve";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input") );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "CURVE_NAME" ,  VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "scale" ,VPT_FLOAT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "isWrap", VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "isIntegral", VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Util;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* object */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Object";
        nodeFuture.nodeId_ = SNT_OBJECT;
        nodeFuture.description_ = "Add Scene Object";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("VSC0",NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("VSC1",NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("PSC0",NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("PSC1",NL_ET_Matrix) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "groupId" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "MESH_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "numObj" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        nodeFuture.outEdgeType_ = NL_ET_None;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* SNT_DEBUG_CAMERA */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "DevCam";
        nodeFuture.nodeId_ = SNT_DEBUG_CAMERA;
        nodeFuture.description_ = "DebugCamera";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("default",NL_ET_Matrix) );
        nodeFuture.nodeType_ = NodeType_Util;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* SNT_DEBUG_CAMERA */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "DevCamPos";
        nodeFuture.nodeId_ = SNT_DEBUG_CAMERA_POS;
        nodeFuture.description_ = "DebugCameraPos";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("default",NL_ET_Vec4) );
        nodeFuture.nodeType_ = NodeType_Util;
        nodeFuture.outEdgeType_ = NL_ET_Vec4;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matView";
        nodeFuture.nodeId_ = SNT_MATRIX_VIEW;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("eyePos") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("lookAt") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("upDir") );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matProj";
        nodeFuture.nodeId_ = SNT_MATRIX_PROJ;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("zn") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("zf") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("aspect") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("fovy") );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    {
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matProjOrtho";
        nodeFuture.nodeId_ = SNT_MATRIX_PROJ_ORTHO;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("zn") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("zf") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("w") );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("h") );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matWorld";
        nodeFuture.nodeId_ = SNT_MATRIX_WORLD;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("trance", NL_ET_Vec4) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("rotation", NL_ET_Vec4 ) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("scale", NL_ET_Vec4 ) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matScale";
        nodeFuture.nodeId_ = SNT_MATRIX_SCALE;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("scale", NL_ET_Vec4) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matTrance";
        nodeFuture.nodeId_ = SNT_MATRIX_TRANCE;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("trance", NL_ET_Vec4) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matRot";
        nodeFuture.nodeId_ = SNT_MATRIX_ROT;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("rotation", NL_ET_Vec4) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    {/**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matRotZ";
        nodeFuture.nodeId_ = SNT_MATRIX_ROTZ;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("orig", NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("orig", NL_ET_Vec4) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matMul";
        nodeFuture.nodeId_ = SNT_MATRIX_MUL;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src0", NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src1", NL_ET_Matrix) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matInv";
        nodeFuture.nodeId_ = SNT_MATRIX_INV;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src", NL_ET_Matrix) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "matPack";
        nodeFuture.nodeId_ = SNT_MATRIX_PACK;
        nodeFuture.outEdgeType_ = NL_ET_Matrix;
        nodeFuture.description_ = "";
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src0", NL_ET_Vec4) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src1", NL_ET_Vec4) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src2", NL_ET_Vec4) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("src3", NL_ET_Vec4) );
        nodeFuture.nodeType_ = NodeType_Matrix;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* begin render */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Begin";
        nodeFuture.nodeId_ = SNT_BEGINRENDER;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* end render */
        NodeFeature nodeFuture;
        nodeFuture.nodeId_ = SNT_ENDRENDER;
        nodeFuture.nodeName_ = "End";
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_None;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input", NL_ET_Render) );
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* pass */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Pass";
        nodeFuture.nodeId_ = SNT_PASS;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input", NL_ET_Render) );
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Clear RT */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "ClearRT";
        nodeFuture.nodeId_ = SNT_CLEARRT;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input", NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "RT_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "r" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "g" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "b" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* Clear RT */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "ClearDS";
        nodeFuture.nodeId_ = SNT_CLEARDS;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input", NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "index" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* RC_CLEAR_DEPTH_STENCIL */
    }
    { /* Set Z State */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Z State";
        nodeFuture.nodeId_ = SNT_SET_Z_STATE;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input", NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "ZState" , VPT_STRING_TO_ID ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
        /**/
        setId("ZState","ReadWrite",0);
        setId("ZState","ReadOnly",1);
        setId("ZState","NotUse",2);
    }
    { /* Set Alpha State */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Alpha State";
        nodeFuture.nodeId_ = SNT_SET_ALPHA_STATE;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input", NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "AlphaState" , VPT_STRING_TO_ID ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
        /**/
        setId("AlphaState","Disable",0);
        setId("AlphaState","Add",1);
        setId("AlphaState","Alpha",2);
    }
    { /* Set Rendertargeet */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Set RT";
        nodeFuture.nodeId_ = SNT_SETRENDERTARGET;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "RT_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "RT_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "RT_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "DS_NAME"  , VPT_STRING_TO_ID ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "SetCB";
        nodeFuture.nodeId_ = SNT_SET_CB;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("C0",NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("C1",NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("C2",NL_ET_Matrix) );
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("C3",NL_ET_Matrix) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "SH_TYPE" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "start idx" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "num const" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
        /* ShaderTypeÇ…èÄãí */
        setId( "SH_TYPE", "VS", ShaderType_VS );
        setId( "SH_TYPE", "GS", ShaderType_GS );
        setId( "SH_TYPE", "PS", ShaderType_PS );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Set Tex";
        nodeFuture.nodeId_ = SNT_SET_TEX;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "TS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "index" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    {
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "Set Tex RT";
        nodeFuture.nodeId_ = SNT_SET_TEX_RT;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "RT_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "set index" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "ScreenPoly2";
        nodeFuture.nodeId_ = SNT_RENDER_SCREEN2;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "PS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /**/
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "ScreenPoly3";
        nodeFuture.nodeId_ = SNT_RENDER_SCREEN3;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "PS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "VS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* SNT_DRAWGROUP */
        NodeFeature nodeFuture;
        nodeFuture.nodeName_ = "DrawGroup";
        nodeFuture.nodeId_ = SNT_DRAWGROUP;
        nodeFuture.description_ = "";
        nodeFuture.outEdgeType_ = NL_ET_Render;
        nodeFuture.inputValveProps_.push_back( NodeFeature::ValveProp("input",NL_ET_Render) );
        nodeFuture.props_.push_back( NodeFeature::PropProp( "groupId" , VPT_INT ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "VS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "GS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "PS_NAME" , VPT_STRING_TO_ID ));
        nodeFuture.props_.push_back( NodeFeature::PropProp( "Topology" , VPT_INT ));
        nodeFuture.nodeType_ = NodeType_Render;
        desc.nodeFeatures_.push_back( nodeFuture );
    }
    { /* RS_SET_Z_STATE */
    }
    { /* RS_SET_ALPHA_STATE */
    }
    return desc;
}
