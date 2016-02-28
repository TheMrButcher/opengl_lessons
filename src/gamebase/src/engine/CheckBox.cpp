#include <stdafx.h>
#include <gamebase/engine/CheckBox.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

CheckBox::CheckBox(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<CheckBoxSkin>& skin)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_checked(false)
{
    m_skin->setChecked(false);
}

void CheckBox::setChecked(bool status)
{
    if (m_checked != status) {
        m_checked = status;
        m_skin->setChecked(status);
        if (m_callback)
            m_callback(status);
    }
}

void CheckBox::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        setChecked(!m_checked);
    }

    m_selectionState = state;
    m_skin->setSelectionState(state);
}

void CheckBox::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject(m_skin.get());
    builder->registerProperty("check", &m_checked,
        std::bind(&CheckBox::setChecked, this, std::placeholders::_1));
}

void CheckBox::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "checked" << m_checked;
}

std::unique_ptr<IObject> deserializeCheckBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<CheckBoxSkin>, skin);
    DESERIALIZE(bool, checked);
    std::unique_ptr<CheckBox> result(new CheckBox(position, skin));
    result->setChecked(checked);
    return std::move(result);
}

REGISTER_CLASS(CheckBox);

}
