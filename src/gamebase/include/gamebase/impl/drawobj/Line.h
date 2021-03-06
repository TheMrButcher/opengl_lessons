/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Direction.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Line : public Drawable, public IPositionable,
    public Registrable, public ISerializable {
public:
    Line(const std::shared_ptr<Drawable>& skin);

    const Vec2& p1() const { return m_p1; }
    void setP1(const Vec2& v) { m_p1 = v; update(); }
    const Vec2& start() const { return m_p1; }
    void setStart(const Vec2& v) { setP1(v); }

    const Vec2& p2() const { return m_p2; }
    void setP2(const Vec2& v) { m_p2 = v; update(); }
    const Vec2& end() const { return m_p2; }
    void setEnd(const Vec2& v) { setP2(v); }

    float width() const { return m_width; }
    void setWidth(float value) { m_width = value; update(); }

    void set(const Vec2& p1, const Vec2& p2, float width);
    void set(const Vec2& p1, const Vec2& p2) { set(p1, p2, m_width); }

    Direction::Enum stretchDir() const { return m_stretchDir; }
    void setStretchDir(Direction::Enum value) { m_stretchDir = value; update(); }

    virtual Transform2 position() const override { return m_transform; }
    
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override
    {
        if (m_skin)
            m_skin->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    void update();
    void updateBox();

    std::shared_ptr<Drawable> m_skin;
    Vec2 m_p1;
    Vec2 m_p2;
    float m_width;
    Direction::Enum m_stretchDir;
    
    bool m_inited;
    BoundingBox m_box;
    Transform2 m_transform;
};

} }
