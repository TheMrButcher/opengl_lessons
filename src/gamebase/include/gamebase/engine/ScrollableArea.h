#pragma once

#include <gamebase/engine/ScrollableAreaSkin.h>

namespace gamebase {

class GAMEBASE_API ScrollableArea : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable {
public:
    ScrollableArea(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<ScrollableAreaSkin>& skin);
    
    const ObjectsCollection& objects() const { return m_objects; }
    ObjectsCollection& objects() { return m_objects; }

    void setScrollStep(float step)
    {
        if (m_horScroll)
            m_horScroll->setStepSize(step);
        if (m_vertScroll)
            m_vertScroll->setStepSize(step);
    }

    void setAssociatedSelectable(ISelectable* selectable);

    bool recountObjectsBoxes() const { return m_recountObjectsBoxes; }
    void setRecountObjectsBoxes(bool value) { m_recountObjectsBoxes = value; }

    BoundingBox areaBox() const { return m_skin->areaBox(); }

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<ScrollableAreaSkin> m_skin;
    class ScrollPosition;
    std::shared_ptr<ScrollPosition> m_scrollOffset;
    ObjectsCollection m_objects;
    ObjectsCollection m_sysObjects;
    std::shared_ptr<ScrollBar> m_horScroll;
    std::shared_ptr<ScrollBar> m_vertScroll;
    bool m_recountObjectsBoxes;
};

}