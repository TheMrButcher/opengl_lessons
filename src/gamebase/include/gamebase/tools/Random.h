/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <cstdlib>

namespace gamebase {

GAMEBASE_API void randomize();

inline int randomInt(int minVal, int maxVal)
{
    return minVal + rand() % (maxVal - minVal + 1);
}

inline float randomFloat()
{
    return static_cast<float>(rand()) / RAND_MAX;
}

inline float randomFloat(float minVal, float maxVal)
{
    return minVal + randomFloat() * (maxVal - minVal);
}

inline double randomDouble()
{
    return static_cast<double>(rand()) / RAND_MAX;
}

inline double randomDouble(double minVal, double maxVal)
{
    return minVal + randomDouble() * (maxVal - minVal);
}

}
