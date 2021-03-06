/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/ButtonListSkin.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ButtonList : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public IScrollable, public ISerializable {
public:
    ButtonList(
        const std::shared_ptr<ButtonListSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void addButton(const std::shared_ptr<Button>& button);
    const std::vector<std::shared_ptr<IObject>>& buttons() const { return m_list.objects(); }

    void setAssociatedSelectable(ISelectable* selectable);

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
	virtual IScrollable* findScrollableByPoint(const Vec2& point) override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

	virtual void applyScroll(float scroll) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<ButtonListSkin> m_skin;
    class ScrollPosition;
    std::shared_ptr<ScrollPosition> m_scrollOffset;
    ObjectsCollection m_list;
    std::shared_ptr<ScrollBar> m_scroll;
    BoundingBox m_box;
};

} }
