/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/text/FontStorage.h>
#include <gamebase/impl/text/TextBank.h>
#include <gamebase/impl/audio/SoundLibrary.h>

namespace gamebase { namespace impl {

struct GlobalResources {
    FontStorage fontStorage;
    TextBank textBank;
    SoundLibrary soundLibrary;
};

void loadGlobalResources();
GlobalResources& globalResources();

} }
