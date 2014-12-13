#pragma once

#include <gamebase/engine/ClickingInputProcessor.h>
#include <gamebase/engine/ProjectionTransform.h>
#include <gamebase/math/Vector2.h>

namespace gamebase {

class InputProcessor {
public:
    void setMousePosition(int x, int y)
    {
        setMousePosition(convertMouseCoords(x, y));
    }

    void setMousePosition(const Vec2& normalizedPosition)
    {
        m_mousePosition = normalizedPosition;
    }

    const Vec2& mousePosition() const
    {
        return m_mousePosition;
    }

    void step()
    {
        keys.step();
        specialKeys.step();
        mouseButtons.step();
    }

    void reset()
    {
        keys.reset();
        specialKeys.reset();
        mouseButtons.reset();
    }

public:
    ClickingInputProcessor<unsigned char> keys;
    ClickingInputProcessor<int> specialKeys;
    ClickingInputProcessor<int> mouseButtons;

private:
    Vec2 m_mousePosition;
};

}
