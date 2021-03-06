/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "FontSFML.h"
#include "TextRendererSFML.h"
#include <gamebase/tools/Exception.h>
#include <gamebase/math/Math.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>

namespace gamebase { namespace impl {

FontSFML::FontSFML(
    const std::shared_ptr<sf::Font>& font,
    const std::string& familyName,
    unsigned int size,
    bool makeBold,
    bool makeItalic,
    float outlineWidth)
    : m_font(font)
    , m_familyName(familyName)
    , m_size(size)
    , m_style(0)
    , m_outlineWidth(outlineWidth)
{
    if (makeBold)
        m_style = m_style | sf::Text::Bold;
    if (makeItalic)
        m_style = m_style | sf::Text::Italic;
}

float FontSFML::offsetY() const
{
    if (!m_offsetY) {
        sf::Text text(sf::String("Hhtl"), *m_font, fontSizeInt());
        text.setOutlineThickness(m_outlineWidth);
        text.setStyle(m_style);
        m_offsetY = -text.getLocalBounds().top;
    }
    return *m_offsetY;
}

std::shared_ptr<ITextRenderer> FontSFML::makeRenderer() const
{
    return std::make_shared<TextRendererSFML>(this);
}

float FontSFML::ascent() const
{
    if (!m_ascent) {
        auto ascent = std::max({
            glyph('H').bounds.height,
            glyph('h').bounds.height,
            glyph('t').bounds.height,
            glyph('l').bounds.height });
        if (ascent <= 0)
            ascent = static_cast<float>(m_size);
        m_ascent = ascent;
    }
    return *m_ascent;
}

float FontSFML::descent() const
{
    if (!m_descent) {
        auto descent = std::max({
            -bounds('q').bottom(),
            -bounds('p').bottom() });
        if (descent <= 0)
            descent = 0;
        m_descent = descent;
    }
    return *m_descent;
}

float FontSFML::lineSpacing() const
{
    return m_font->getLineSpacing(m_size);
}

const GLTexture& FontSFML::texture() const
{
    THROW_EX() << "FontSFML::texture is not supported";
}

std::vector<uint32_t> FontSFML::glyphIndices(const std::string& utfStr) const
{
    auto sfmlStr = sf::String::fromUtf8(utfStr.begin(), utfStr.end());
    auto utf32Str = sfmlStr.toUtf32();
    return std::vector<uint32_t>(utf32Str.begin(), utf32Str.end());
}

float FontSFML::advance(uint32_t glyphIndex) const
{
    return glyph(glyphIndex).advance;
}

float FontSFML::kerning(uint32_t glyphIndex1, uint32_t glyphIndex2) const
{
    return m_font->getKerning(glyphIndex1, glyphIndex2, m_size);
}

BoundingBox FontSFML::bounds(uint32_t glyphIndex) const
{
    const auto& glyph = this->glyph(glyphIndex);
    return BoundingBox(
        Vec2(glyph.bounds.left, -glyph.bounds.top),
        Vec2(glyph.bounds.left + glyph.bounds.width, -glyph.bounds.top - glyph.bounds.height));
}

BoundingBox FontSFML::glyphTextureRect(uint32_t glyphIndex) const
{
    THROW_EX() << "FontSFML::glyphTextureRect(uint32_t) is not supported";
}

const sf::Glyph& FontSFML::glyph(uint32_t glyphIndex) const
{
    bool bold = m_style & sf::Text::Bold;
    return m_font->getGlyph(glyphIndex, m_size, bold, m_outlineWidth);
}

} }
