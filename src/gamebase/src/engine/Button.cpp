#include <stdafx.h>
#include <gamebase/engine/Button.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

Button::Button(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
{}

void Button::setSelectionState(SelectionState::Enum state)
{
    bool clicked = false;
    if (state == SelectionState::Selected) {
        state = SelectionState::MouseOn;
        clicked = true;
    }
    m_selectionState = state;
    m_skin->setSelectionState(state);
    if (clicked && m_callback)
        m_callback();
}

void Button::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject(m_skin.get());
}

void Button::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeButton(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, skin);
    return std::unique_ptr<IObject>(new Button(skin, position));
}

REGISTER_CLASS(Button);

}
