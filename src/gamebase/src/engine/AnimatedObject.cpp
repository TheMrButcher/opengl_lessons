#include <stdafx.h>
#include <gamebase/engine/AnimatedObject.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

void AnimatedObject::serialize(Serializer& s) const
{
    s << "box" << m_box << "geometry" << m_geom
        << "animations" << m_animations << "elements" << m_skinElements.objects();
}

void deserializeAnimatedObjectElements(
    Deserializer& deserializer, AnimatedObject* obj)
{
    typedef std::map<SelectionState::Enum, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);
    for (auto it = animations.begin(); it != animations.end(); ++it)
        obj->setTransitAnimation(it->first, it->second);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        obj->addElement(*it);
}

}
