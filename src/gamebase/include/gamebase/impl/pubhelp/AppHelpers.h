/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/AppImpl.h>
#include <gamebase/impl/pubhelp/Helpers.h>

#define GAMEBASE_DEFINE_APP_PIMPL() \
public: \
    App() : m_impl(new impl::AppImpl(this)) {} \
private: \
    std::shared_ptr<impl::AppImpl> m_impl

#define GAMEBASE_DEFINE_APP_INPUT_PIMPL() \
public: \
    void setImpl(const impl::InputRegister* inputReg) { m_impl = inputReg; } \
private: \
    const impl::InputRegister* m_impl
