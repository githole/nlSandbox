#pragma once

#include "shared/nlTypedef.h"
#include "shared/curve/nlCurve.hpp"

enum nlCurveSeliazerType
{
    nlCST_Binary, /* �񐄏� */
    nlCST_KDF,    /* SysK�c�[������̈ڐA�p */
    nlCST_XML,    /* xml�`�� */
};

/**
 * @brief Curve
 */
bool serializeCurve( const char* fileName, const nlCurve& path, nlCurveSeliazerType seliazerType );
/**
 * @brief Curve
 */
bool deserializeCurve( const char* fileName, nlCurve& newCurve, nlCurveSeliazerType seliazerType );