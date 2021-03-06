/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/gameobj/PositionElement.h>
#include <gamebase/impl/relbox/IResizable.h>
#include <gamebase/impl/engine/Identifiable.h>

namespace gamebase { namespace impl {

class ILayer;

class GAMEBASE_API InactiveObjectConstruct : public IPositionable, public Drawable,
    public ISerializable, public Registrable, public Identifiable, public IResizable {
public:
    InactiveObjectConstruct(
        const std::shared_ptr<Drawable>& drawable,
        const std::shared_ptr<PositionElement>& position = nullptr);
    
    Drawable* skin() const { return m_drawable.get(); }
    void kill();
    void moveToLayer(const std::string& dstLayerName);
    void moveToLayer(ILayer* dstLayer);
    virtual void setFixedBox(float width, float height) override;

    Vec2 getOffset() const { return m_posElement->getOffset(); }
    void setOffset(const Vec2& v) { m_posElement->setOffset(v); }
    void setOffset(float x, float y) { setOffset(Vec2(x, y)); }

    float scale() const { return m_posElement->scale(); }
    void setScale(float scale) { m_posElement->setScale(scale); }
    void setScale(float scaleX, float scaleY) { m_posElement->setScale(scaleX, scaleY); }

    float scaleX() const { return m_posElement->scaleX(); }
    void setScaleX(float scale) { m_posElement->setScaleX(scale); }

    float scaleY() const { return m_posElement->scaleY(); }
    void setScaleY(float scale) { m_posElement->setScaleY(scale); }

    float angle() const { return m_posElement->angle(); }
    void setAngle(float angle) { m_posElement->setAngle(angle); }

    virtual Transform2 position() const override { return m_posElement->position(); }

    virtual void loadResources() override
    {
        m_drawable->loadResources();
    }

    virtual void drawAt(const Transform2& position) const
    {
        m_drawable->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_drawable->setBox(allowedBox);
        m_parentBox = allowedBox;
    }

    virtual BoundingBox box() const override
    {
        return m_drawable->movedBox();
    }

    BoundingBox parentBox() const
    {
        return m_parentBox;
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& serializer) const override;

protected:
    std::shared_ptr<Drawable> m_drawable;
    std::shared_ptr<PositionElement> m_posElement;
    BoundingBox m_parentBox;
};

typedef InactiveObjectConstruct StaticGameObj;

} }
