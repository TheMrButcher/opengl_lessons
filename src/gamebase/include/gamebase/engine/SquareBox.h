#pragma once

#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/serial/ISerializable.h>
#include <algorithm>

namespace gamebase {

class SquareBox : public IRelativeBox, public ISerializable {
public:
    virtual BoundingBox count(const BoundingBox& parentBox) const override
    {
        auto minSize = std::min(parentBox.width(), parentBox.height());
        return BoundingBox(
            Vec2(-0.5f * minSize, -0.5f * minSize),
            Vec2(0.5f * minSize, 0.5f * minSize));
    }

    virtual void serialize(Serializer&) const override {}
};

}
