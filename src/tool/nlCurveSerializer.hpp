#pragma once

#include "shared/nlTypedef.h"
#include "shared/curve/nlCurve.hpp"

enum nlCurveSeliazerType
{
    nlCST_Binary, /* 非推奨 */
    nlCST_KDF,    /* SysKツールからの移植用 */
    nlCST_XML,    /* xml形式 */
};

/**
 * @brief Curve
 */
bool serializeCurve( const char* fileName, const nlCurve& path, nlCurveSeliazerType seliazerType );
/**
 * @brief Curve
 */
bool deserializeCurve( const char* fileName, nlCurve& newCurve, nlCurveSeliazerType seliazerType );