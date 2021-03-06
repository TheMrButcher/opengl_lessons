/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/IndexBuffer.h>
#include <gamebase/tools/Exception.h>
#include <functional>

namespace gamebase { namespace impl {

void IndexBuffer::bind() const
{
    if (m_size == 0 || !m_id)
        THROW_EX() << "Can't bind empty IndexBuffer";
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_id);
}

void IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::init(const uint16_t* indices, size_t size)
{
    m_size = size;
    auto* id = new GLuint(0);
    m_id.reset(id, [](auto* id) { glDeleteBuffers(1, id); });
    glGenBuffers(1, m_id.get());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint16_t), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} }
