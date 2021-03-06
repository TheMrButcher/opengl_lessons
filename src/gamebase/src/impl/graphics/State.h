/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/OneColorProgram.h>
#include <gamebase/impl/graphics/LineProgram.h>
#include <gamebase/impl/graphics/TextureProgram.h>
#include <gamebase/impl/graphics/PatternProgram.h>
#include <gamebase/impl/graphics/MultiColorProgram.h>
#include <gamebase/impl/graphics/ColoredTextureProgram.h>

namespace gamebase { namespace impl {

struct State {
    int width;
    int height;

    struct {
        OneColorProgram oneColor;
        LineProgram line;
        TextureProgram texture;
        PatternProgram pattern;
        MultiColorProgram multiColor;
		ColoredTextureProgram coloredTexture;
    } programs;
};

void initState(int width, int height);
const State& state();

} }
