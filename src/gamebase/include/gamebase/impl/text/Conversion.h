/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase { namespace impl {

GAMEBASE_API std::string convertToUtf8(const std::string& localStr);

GAMEBASE_API std::string convertToLocal(const std::string& utf8Str);

GAMEBASE_API std::string normalizeUtf8(const std::string& utf8Str);

} }
