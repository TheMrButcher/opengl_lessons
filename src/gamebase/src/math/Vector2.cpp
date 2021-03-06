/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/math/Vector2.h>

namespace gamebase {

float Vec2::angle() const
{
    if (isZero())
        return 0;
    return std::atan2(y, x);
}

void Vec2::rotate(float angle)
{
    RotationMatrix2 rotMatrix(angle);
    *this = rotMatrix * *this;
}

Vec2 polarVec(float length, float angle)
{
    return Vec2(length * std::cos(angle), length * std::sin(angle));
}

}
