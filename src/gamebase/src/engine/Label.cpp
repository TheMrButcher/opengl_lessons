#include <stdafx.h>
#include <gamebase/engine/Label.h>
#include <gamebase/text/TextGeometry.h>
#include <gamebase/graphics/TextureProgram.h>

namespace gamebase {
    
void Label::setTextAndLoad(const std::string& text)
{
    setText(text);
    if (m_rect.isValid())
        loadResources();
}

void Label::loadResources()
{
    m_font = m_alignProps.font.get();
    try {
        m_buffers = createTextGeometryBuffers(
            m_text, m_alignProps, m_rect);
    } catch (std::exception& ex) {
        std::cout << "Error while trying to load text \"" << m_text << "\" to Label"
            ". Reason: " << ex.what() << std::endl;
        return;
    }
}

void Label::drawAt(const Transform2& position) const
{
    if (m_text.empty())
        return;
    const TextureProgram& program = textureProgram();
    program.transform = position;
    program.texture = m_font->texture();
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void Label::setBox(const BoundingBox& allowedBox)
{
    m_rect = allowedBox;
    if (m_adjustSize) {
        auto alignedText = alignText(m_text, m_alignProps, allowedBox);
        BoundingBox extent;
        for (auto it = alignedText.begin(); it != alignedText.end(); ++it)
            extent.enlarge(it->bbox);
        if (extent.isValid())
            m_rect = extent;
        else
            m_rect.topRight = m_rect.bottomLeft;
    }
}

}
