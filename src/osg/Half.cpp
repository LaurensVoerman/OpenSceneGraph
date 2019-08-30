/* -*-c++-*- OpenSceneGraph - Copyright (C) 2019 Laurens Voerman
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <osg/Half>

namespace osg {

/*
float 1/8/23 ebias 127
half  1/5/10 ebias 15
*/
union flt32 {
    float flt;
    unsigned __int32 bits;
};
float halfToFloat(half_float input)
{
    bool negative = ((input & 0x8000) == 0x8000);
    int exp = (input >> 10) & 0x1f;
    int mantissa = input & 0x03ff;
    flt32 result;
    result.bits = negative << 31;
    if (exp == 0) {
        if (mantissa == 0) return result.flt;
        //subnormal number
        while ((mantissa & 0x0400) == 0) {
            mantissa <<= 1;
            --exp;
        }
        mantissa &= 0x03ff;//remove implicit 1
    }
    exp += 112;
    if (exp == 143) exp = 255;//inf/snan/qnan
    result.bits |= (exp << 23) | (mantissa << 13);
    return result.flt;
}
half_float floatToHalf(float input)
{
    flt32 in;
    in.flt = input;
    bool negative = ((in.bits & 0x80000000) == 0x80000000);
    half_float result = negative ? 0x8000 : 0;
    if ((in.bits & 0x7fffffff) == 0) {
        return result;
    }
    int exp = (in.bits >> 23) & 0xff;
    int mantissa = in.bits & 0x7fffff;
    if (exp != 0xff) {// number
        exp -= 112;
        if (exp > 0) {//(input > 0.000030517578125f) {// 2^-15
            if (exp < 31) {
                return result | (exp << 10) | (mantissa >> 13);
            } else {
                return result | 0x7bff;//HALF_MAX
            }
        } else {
            if (exp >= -10) {// subnormal numbers
                ++exp;
                mantissa >>= 14;
                mantissa &= 0x200;//add implicit 1
                while (exp < 0) {
                    ++exp;
                    mantissa >>= 1;
                }
                return result | mantissa;
            }
            return result;//+0.0f or -0.0f
        }
    } else {//inf/snan/qnan
        if (mantissa == 0) {
            return result | 0x7c00;//-Inf Inf
        } else {
            mantissa >>= 13;
            if (mantissa == 0) mantissa = 0x01ff;//sNan
            return result | mantissa;
        }
    }
}

}

