/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/tools/Delayed.h>
#include <functional>

namespace gamebase { namespace impl {

class GAMEBASE_API ToolTip : public OffsettedPosition, public Drawable, public Registrable, public ISerializable {
public:
    ToolTip(
        const std::shared_ptr<Drawable>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setSourcePoint(const Vec2& sourcePoint);

    bool isRelativeToSourcePoint() const;
    void setRelativeToSourcePoint(bool value);

    virtual void loadResources() override
    {
        m_skin->loadResources();
    }

    virtual void drawAt(const Transform2& position) const
    {
        m_skin->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_skin->setBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<Drawable> m_skin;
};

} }
