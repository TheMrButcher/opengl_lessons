/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/LinearLayoutSkin.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relpos/AlignEnums.h>
#include <gamebase/impl/engine/RelativeValue.h>
#include <gamebase/impl/engine/Adjustment.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API VerticalLayoutSkin : public LinearLayoutSkin, public ISerializable {
public:
    VerticalLayoutSkin(const std::shared_ptr<IRelativeBox>& box)
        : m_box(box)
        , m_padding(RelType::Pixels, 0)
        , m_adjustment(Adjustment::None)
        , m_align(HorAlign::Left)
    {}

    const std::shared_ptr<IRelativeBox>& relativeBox() const { return m_box; }

    const RelativeValue& padding() const { return m_padding; }
    void setPadding(const RelativeValue& padding) { m_padding = padding; }

    Adjustment::Enum adjustment() const { return m_adjustment; }
    void setAdjustment(Adjustment::Enum value) { m_adjustment = value; }

    HorAlign::Enum align() const { return m_align; }
    void setAlign(HorAlign::Enum align) { m_align = align; }

    virtual BoundingBox listBox() const override
    {
        return m_box->get();
    }

    virtual Direction::Enum direction() const override
    {
        return Direction::Vertical;
    }

    virtual std::shared_ptr<IRelativeOffset> createOffset(size_t index) const override;
    virtual void setExtent(const BoundingBox& box) override;
    virtual void setFixedBox(float width, float height) override;

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_curBox = m_box->get();
    }
    virtual BoundingBox box() const override
    {
        return m_curBox;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override {}
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    BoundingBox m_curBox;
    RelativeValue m_padding;
    Adjustment::Enum m_adjustment;
    HorAlign::Enum m_align;
};

} }
