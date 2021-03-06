/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/reg/IRegistrable.h>
#include <gamebase/impl/reg/ValueLink.h>
#include <gamebase/impl/reg/ValueNotifyingLink.h>

namespace gamebase { namespace impl {

PropertiesRegisterBuilder g_registryBuilder;

PropertiesRegisterBuilder::PropertiesRegisterBuilder()
    : m_current(nullptr)
{}

void PropertiesRegisterBuilder::registerObject(IObject* obj)
{
    if (auto* registrable = dynamic_cast<IRegistrable*>(obj)) {
        auto name = registrable->name();
        registerInCurrent(name, obj);
        buildRegister(name, registrable);
    } else {
        registerInCurrent("", obj);
    }
}

void PropertiesRegisterBuilder::registerObject(const std::string& name, IObject* obj)
{
    std::string regName = name;
    if (auto* registrable = dynamic_cast<IRegistrable*>(obj)) {
        if (!registrable->name().empty()) {
            registerInCurrent(registrable->name(), obj);
            if (name.empty())
                regName = registrable->name();
        }
    }
    if (regName.empty() || !name.empty())
        registerInCurrent(name, obj);
    if (auto* registrable = dynamic_cast<IRegistrable*>(obj))
        buildRegister(regName, registrable);
}


void PropertiesRegisterBuilder::registerColor(
    const std::string& name,
    GLColor* prop,
    const std::function<void()>& notifier)
{
    registerProperty<GLColor>(name, prop, notifier);
    registerProperty<float>(name + "R", &prop->r, notifier);
    registerProperty<float>(name + "G", &prop->g, notifier);
    registerProperty<float>(name + "B", &prop->b, notifier);
    registerProperty<float>(name + "A", &prop->a, notifier);
}

void PropertiesRegisterBuilder::registerVec2(
    const std::string& name,
    Vec2* prop,
    const std::function<void()>& notifier)
{
    registerProperty<Vec2>(name, prop, notifier);
    registerProperty<float>(name + "X", &prop->x, notifier);
    registerProperty<float>(name + "Y", &prop->y, notifier);
}

PropertiesRegisterBuilder::PropertiesRegisterBuilder(IRegistrable* current)
    : m_current(current)
{}

void PropertiesRegisterBuilder::registerInCurrent(
    const std::string& name, IObject* obj)
{
    if (m_current)
        m_current->properties().add(name, obj);
}

void PropertiesRegisterBuilder::buildRegister(const std::string& regName, IRegistrable* registrable)
{
    PropertiesRegisterBuilder subbuilder(registrable);
    auto& props = registrable->properties();
    props.m_current = registrable;
    props.m_parent = m_current;
    if (!props.empty())
        return;
    try {
        registrable->registerObject(&subbuilder);
    } catch (std::exception& ex) {
        THROW_EX() << "Can't register object " << regName << ". Reason: " << ex.what();
    }
}

} }
