/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/SimpleRectangleTextBoxSkin.h>
#include <gamebase/impl/geom/IdenticGeometry.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

SimpleRectangleTextBoxSkin::SimpleRectangleTextBoxSkin(
    const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
	, m_geom(std::make_shared<IdenticGeometry>())
    , m_cursorOffsetX(0)
    , m_cursorPos(0)
    , m_drawCursor(false)
    , m_loaded(false)
    , m_label(std::make_shared<OffsettedBox>())
{
	m_border.setColor(Color(0, 0, 0, 1));
	m_fill.setColor(Color(0.7f, 0.7f, 0.7f, 1));
	setTextColor(Color(0, 0, 0, 1));
	m_label.setLimited(false);
	setFont(FontDesc());
}

void SimpleRectangleTextBoxSkin::setTextColor(const Color& color)
{
	m_label.setColor(color);
	m_cursor.setColor(color);
}

void SimpleRectangleTextBoxSkin::setSelection(size_t startIndex, size_t endIndex)
{
    if (startIndex == endIndex) {
        m_cursorPos = startIndex;
        m_drawCursor = true;
    } else {
        m_drawCursor = false;
    }
    m_label.setSelection(startIndex, endIndex);
}

void SimpleRectangleTextBoxSkin::loadResources()
{
    if (!m_loaded) {
        m_border.loadResources();
		m_fill.loadResources();
        m_loaded = true;
    }

    m_label.loadResources();
    BoundingBox charBox = m_label.textGeometry().at(m_cursorPos).position;
    m_cursor.setX(charBox.bottomLeft.x + m_cursorOffsetX);
    m_cursor.setYRange(charBox.bottomLeft.y, charBox.topRight.y);
    m_cursor.loadResources();
}

void SimpleRectangleTextBoxSkin::drawAt(const Transform2& position) const
{
	m_border.draw(position);
	m_fill.draw(position);
    m_label.draw(position);
    if (m_selectionState == SelectionState::Selected && m_drawCursor)
        m_cursor.draw(position);
}

void SimpleRectangleTextBoxSkin::setBox(const BoundingBox& allowedBox)
{
	static const float PADDING = 2.0f;

	m_box->setParentBox(allowedBox);
	BoundingBox box = m_box->get();
    m_geom->setBox(box);

	BoundingBox innerBox = extend(box, -m_borderWidth);
	if (!innerBox.isValid())
		innerBox = BoundingBox(box.center());

	m_border.setBox(box);
	m_fill.setBox(innerBox);

	BoundingBox labelBox = extend(innerBox, -PADDING);
	if (!labelBox.isValid())
		labelBox = innerBox;
    m_label.setBox(labelBox);
	m_loaded = false;
}

void SimpleRectangleTextBoxSkin::serialize(Serializer& s) const
{
	s   << "fillColor" << m_fill.color()
		<< "borderWidth" << m_borderWidth << "borderColor" << m_border.color()
		<< "selectionColor" << m_label.selectionColor()
		<< "textColor" << m_label.color() << "font" << m_label.font()
		<< "box" << m_box;
}

std::unique_ptr<IObject> deserializeSimpleRectangleTextBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
	DESERIALIZE(Color, fillColor);
	DESERIALIZE(float, borderWidth);
	DESERIALIZE(Color, borderColor);
	DESERIALIZE(Color, textColor);
	DESERIALIZE(FontDesc, font);
	DESERIALIZE(Color, selectionColor);
    std::unique_ptr<SimpleRectangleTextBoxSkin> result(new SimpleRectangleTextBoxSkin(box));
	result->setFillColor(fillColor);
	result->setBorderWidth(borderWidth);
	result->setBorderColor(borderColor);
	result->setTextColor(textColor);
	result->setFont(font);
	result->setSelectionColor(selectionColor);
    return std::move(result);
}

REGISTER_CLASS(SimpleRectangleTextBoxSkin);

} }