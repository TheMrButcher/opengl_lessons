/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/ClickableElement.h>
#include <gamebase/impl/gameobj/PressableElement.h>
#include <gamebase/impl/gameobj/PressableAndClickableElement.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ClickableElement::ClickableElement()
    : m_isClicked(false)
    , m_clickTime(DEFAULT_CLICK_TIME)
    , m_isJustOutpressed(false)
{}

void ClickableElement::setSelectionState(SelectionState::Enum state)
{
    m_isClicked = false;

    if (state == SelectionState::Pressed)
        m_timer.start();

    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        if (m_timer.time() < m_clickTime)
            m_isClicked = true;
    }

    if (m_selectionState == SelectionState::Pressed && state != m_selectionState)
        m_isJustOutpressed = true;

    m_selectionState = state;
    if (m_transitionCallback)
        m_transitionCallback(state);
}

void ClickableElement::serialize(Serializer& s) const
{
    s << "clickTime" << m_clickTime;
}

std::unique_ptr<IObject> deserializeClickableElement(Deserializer& deserializer)
{
    DESERIALIZE_OPT(Time, clickTime, ClickableElement::DEFAULT_CLICK_TIME);
    std::unique_ptr<ClickableElement> result(new ClickableElement());
    result->setClickTime(clickTime);
    return std::move(result);
}

REGISTER_CLASS(ClickableElement);

void PressableElement::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected)
        state = SelectionState::MouseOn;

    if (m_selectionState == SelectionState::Pressed && state != m_selectionState)
            m_isJustOutpressed = true;

    m_selectionState = state;
    if (m_transitionCallback)
        m_transitionCallback(state);
}

std::unique_ptr<IObject> deserializePressableElement(Deserializer&)
{
    return std::unique_ptr<PressableElement>(new PressableElement());
}

REGISTER_CLASS(PressableElement);

std::unique_ptr<IObject> deserializePressableAndClickableElement(Deserializer& deserializer)
{
    DESERIALIZE_OPT(Time, clickTime, ClickableElement::DEFAULT_CLICK_TIME);
    std::unique_ptr<PressableAndClickableElement> result(new PressableAndClickableElement());
    result->setClickTime(clickTime);
    return std::move(result);
}

REGISTER_CLASS(PressableAndClickableElement);

} }
