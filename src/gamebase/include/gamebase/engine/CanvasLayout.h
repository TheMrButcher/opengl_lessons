#pragma once

#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>

namespace gamebase {

class GAMEBASE_API CanvasLayout : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable {
public:
    CanvasLayout(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    int addObject(const std::shared_ptr<IObject>& obj);
    void replaceObject(int id, const std::shared_ptr<IObject>& obj);
    void removeObject(int id);
    void clear();

    const std::vector<std::shared_ptr<IObject>>& objectsAsList() const { return m_list.objects(); }

    void setAssociatedSelectable(ISelectable* selectable);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void load(const std::shared_ptr<IObject>& obj);
    void refill();

    friend std::unique_ptr<IObject> deserializeCanvasLayout(Deserializer&);

    std::shared_ptr<IRelativeBox> m_box;
    std::map<int, std::shared_ptr<IObject>> m_objects;
    ObjectsCollection m_list;
    int m_nextID;
};

}