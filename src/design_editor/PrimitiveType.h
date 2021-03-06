/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

namespace gamebase { namespace editor {

struct PrimitiveType {
    enum Enum {
        Float,
        Double,
        Int,
        UInt,
        Int64,
        UInt64,
        Bool,
        String
    };
};

} }
