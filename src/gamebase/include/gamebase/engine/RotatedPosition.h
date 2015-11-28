#pragma once

#include <gamebase/engine/IPositionable.h>

namespace gamebase {

class RotatedPosition : virtual public IPositionable {
public:
    RotatedPosition()
        : m_angle(0)
    {}

    RotatedPosition(float angle, const Vec2& offset = Vec2())
        : m_pos(RotationMatrix2(angle), offset)
        , m_angle(angle)
    {}

    Vec2 getOffset() const { return position().offset; }
    void setOffset(const Vec2& v) { m_pos.offset = v; }

    float angle() const { return m_angle; }
    void setAngle(float angle)
    {
        if (m_angle == angle)
            return;
        m_angle = angle;
    }

    virtual Transform2 position() const override
    {
        return m_pos;
    }

protected:
    void updateMatrix()
    {
        if (m_angle == 0)
            m_pos.matrix = Matrix2();
        else
            m_pos.matrix = RotationMatrix2(m_angle);
    }

    Transform2 m_pos;
    float m_angle;
};

}