/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ButtonListSkin.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API CommonButtonListSkin : public ButtonListSkin, public ISerializable {
public:
    CommonButtonListSkin(
        const std::shared_ptr<IRelativeBox>& listBox,
        Direction::Enum direction);

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setListWithoutScrollBarsBox(
        const std::shared_ptr<IRelativeBox>& box)
    {
        m_listWithoutScrollBarsBox = box;
    }

    float padding() const { return m_padding; }
    void setPadding(float padding) { m_padding = padding; }

    void setAdjustSize(bool value) { m_adjustSize = value; }
    bool adjustSize(bool value) { return m_adjustSize; }

    void setScrollBarSkin(
        const std::shared_ptr<ScrollBarSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue) const override;
    virtual BoundingBox listBox() const { return m_curListBox; }
    virtual Direction::Enum direction() const { return m_direction; }
    virtual std::shared_ptr<IRelativeOffset> createButtonOffset(size_t index) const override;
    virtual void setSize(float size) override;

    virtual void loadResources() override
    {
        m_skinElements.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_skinElements.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_curListBox;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<IRelativeBox> m_listBox;
    Direction::Enum m_direction;
    std::shared_ptr<IRelativeBox> m_listWithoutScrollBarsBox;
    float m_padding;
    bool m_adjustSize;
    BoundingBox m_curListBox;

    std::shared_ptr<ScrollBarSkin> m_scrollBarSkin;
    std::shared_ptr<IRelativeOffset> m_scrollBarPosition;

    ObjectsCollection m_skinElements;
};

} }
