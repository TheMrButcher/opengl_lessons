#pragma once

#include <gamebase/geom/IRelativeGeometry.h>
#include <gamebase/geom/RectGeometry.h>

namespace gamebase {

class IdenticGeometry : public IRelativeGeometry {
public:
    virtual std::shared_ptr<IGeometry> count(const BoundingBox& box) const override
    {
        return std::make_shared<RectGeometry>(box);
    }
};

}
