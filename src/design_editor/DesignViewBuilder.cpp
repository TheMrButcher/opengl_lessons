#include "DesignViewBuilder.h"
#include "tools.h"
#include <gamebase/engine/TransparentLinearLayoutSkin.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticFilledRect.h>
#include <gamebase/engine/AnimatedTextEditSkin.h>
#include <gamebase/engine/TextEdit.h>
#include <boost/lexical_cast.hpp>

namespace gamebase { namespace editor {

namespace {
std::shared_ptr<LinearLayout> createPropertiesListLayout()
{
    auto skin = std::make_shared<TransparentLinearLayoutSkin>(
        std::make_shared<OffsettedBox>(), Direction::Vertical);
    skin->setPadding(2.0f);
    skin->setAdjustSize(false);
    return std::make_shared<LinearLayout>(
        std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center), skin);
}

std::shared_ptr<LinearLayout> createPropertyLayout()
{
    auto skin = std::make_shared<TransparentLinearLayoutSkin>(
        std::make_shared<OffsettedBox>(), Direction::Horizontal);
    skin->setPadding(5.0f);
    skin->setAdjustSize(true);
    return std::make_shared<LinearLayout>(nullptr, skin);
}

std::shared_ptr<PressableButton> createObjectButton(const std::string& name)
{
    auto skin = createButtonSkin(200.0f, 20.0f, name);
    auto button = std::make_shared<PressableButton>(nullptr, skin);
    button->setUnpressOnFocusLost(false);
    return button;
}

std::shared_ptr<StaticLabel> createLabel(const std::string& text)
{
    auto label = std::make_shared<StaticLabel>(
        std::make_shared<FixedBox>(200.0f, 20.0f));
    AlignProperties alignProperties;
    alignProperties.horAlign = HorAlign::Center;
    alignProperties.vertAlign = VertAlign::Center;
    alignProperties.enableStacking = false;
    label->setAdjustSize(false);
    label->setText(text + ":");
    return label;
}

std::shared_ptr<TextEdit> createTextEdit()
{
     auto skin = std::make_shared<AnimatedTextEditSkin>(
        std::make_shared<FixedBox>(200.0f, 20.0f));

    skin->label().setRelativeBox(std::make_shared<RelativeBox>(
        RelativeValue(RelType::ValueMinusPixels, 8.0f),
        RelativeValue(RelType::ValueMinusPixels, 8.0f),
        std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));

    auto border = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    border->setColor(Color(0, 0, 0));
    skin->addElement(border);

    auto fill = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            RelativeValue(RelType::ValueMinusPixels, 4.0f),
            std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center)));
    fill->setColor(Color(1.0f, 1.0f, 1.0f));
    skin->addElement(fill);

    return std::make_shared<TextEdit>(nullptr, skin);
}

class PrimitiveArrayElementSuffix {
public:
    PrimitiveArrayElementSuffix()
    {
        m_suffixes[SerializationTag::Vec2].push_back(".x");
        m_suffixes[SerializationTag::Vec2].push_back(".y");

        m_suffixes[SerializationTag::Matrix2].push_back("[0, 0]");
        m_suffixes[SerializationTag::Matrix2].push_back("[0, 1]");
        m_suffixes[SerializationTag::Matrix2].push_back("[1, 0]");
        m_suffixes[SerializationTag::Matrix2].push_back("[1, 1]");

        m_suffixes[SerializationTag::Transform2].push_back(".m[0, 0]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[0, 1]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[1, 0]");
        m_suffixes[SerializationTag::Transform2].push_back(".m[1, 1]");
        m_suffixes[SerializationTag::Transform2].push_back(".tx");
        m_suffixes[SerializationTag::Transform2].push_back(".ty");

        m_suffixes[SerializationTag::BoundingBox].push_back(".left");
        m_suffixes[SerializationTag::BoundingBox].push_back(".bottom");
        m_suffixes[SerializationTag::BoundingBox].push_back(".right");
        m_suffixes[SerializationTag::BoundingBox].push_back(".top");

        m_suffixes[SerializationTag::Color].push_back(".r");
        m_suffixes[SerializationTag::Color].push_back(".g");
        m_suffixes[SerializationTag::Color].push_back(".b");
        m_suffixes[SerializationTag::Color].push_back(".a");
    }

    const std::string& get(SerializationTag::Type tag, size_t index) const
    {
        return m_suffixes.at(tag).at(index);
    }

private:
    std::map<SerializationTag::Type, std::vector<std::string>> m_suffixes;
};

const PrimitiveArrayElementSuffix PRIMITIVE_ARRAY_ELEMENT_SUFFIX;

std::string mergeStrings(const std::string& str1, const std::string& str2)
{
    bool bothNotEmpty = !str1.empty() && !str2.empty();
    return str1 + (bothNotEmpty ? std::string(" : ") : "") + str2;
}
}

DesignViewBuilder::DesignViewBuilder(
    TreeView& treeView,
    ObjectsSelector& propertiesMenu,
    int rootID)
    : m_treeView(treeView)
    , m_propertiesMenu(propertiesMenu)
{
    Properties props;
    props.id = rootID;
    m_properties.push_back(props);
}

void DesignViewBuilder::writeFloat(const std::string& name, float f)
{
    std::string fullName = propertyName(name);
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        fullName = fullName + PRIMITIVE_ARRAY_ELEMENT_SUFFIX.get(
            m_arrayTypes.back(), m_primitiveElementIndex++);
    }
    addProperty(fullName, "float", boost::lexical_cast<std::string>(f));
}

void DesignViewBuilder::writeDouble(const std::string& name, double d)
{
    addProperty(propertyName(name), "double", boost::lexical_cast<std::string>(d));
}

void DesignViewBuilder::writeInt(const std::string& name, int i)
{
    if (name == COLLECTION_SIZE_TAG)
        return;
    addProperty(propertyName(name), "int", boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeUInt(const std::string& name, unsigned int i)
{
    addProperty(propertyName(name), "unsigned int", boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeInt64(const std::string& name, int64_t i)
{
    addProperty(propertyName(name), "int64", boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeUInt64(const std::string& name, uint64_t i)
{
    addProperty(propertyName(name), "unsigned int64", boost::lexical_cast<std::string>(i));
}

void DesignViewBuilder::writeBool(const std::string& name, bool b)
{
    if (name == EMPTY_TAG) {
        if (m_objTypes.back() == ObjType::Unknown) {
            m_properties.push_back(createProperties(m_curName, "empty"));
            m_objTypes.back() = ObjType::Object;
        }
        return;
    }
    addProperty(propertyName(name), "boolean", boost::lexical_cast<std::string>(b));
}

void DesignViewBuilder::writeString(const std::string& name, const std::string& value)
{
    if (name == TYPE_NAME_TAG) {
        m_properties.push_back(createProperties(m_curName, value));
        m_objTypes.back() = ObjType::Object;
        return;
    }

    addProperty(propertyName(name), "string", value);
}

void DesignViewBuilder::startObject(const std::string& name)
{
    m_objTypes.push_back(ObjType::Unknown);
    m_curName = name;
}

void DesignViewBuilder::finishObject()
{
    if (m_objTypes.back() == ObjType::Object
        || m_objTypes.back() == ObjType::Array
        || m_objTypes.back() == ObjType::Map)
        m_properties.pop_back();
    m_objTypes.pop_back();
}

void DesignViewBuilder::startArray(const std::string& name, SerializationTag::Type tag)
{
    if (tag == SerializationTag::Array) {
        m_properties.push_back(createProperties(m_curName, "array"));
        m_objTypes.back() = ObjType::Array;
        m_arrayTypes.push_back(tag);
        return;
    }

    if (tag == SerializationTag::Keys) {
        m_properties.push_back(createProperties(m_curName, "map"));
        m_objTypes.back() = ObjType::Map;
        m_arrayTypes.push_back(tag);
        m_mapProperties.push_back(MapProperties());
        return;
    }

    if (tag == SerializationTag::Values) {
        m_arrayTypes.push_back(tag);
        return;
    }

    m_curName = name;
    if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::Map)
        m_properties.push_back(currentPropertiesForPrimitive("primitive array"));
    m_objTypes.push_back(ObjType::PrimitiveArray);
    m_primitiveElementIndex = 0;
    m_arrayTypes.push_back(tag);
}

void DesignViewBuilder::finishArray()
{
    if (m_objTypes.back() == ObjType::PrimitiveArray) {
        m_objTypes.pop_back();
        if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::Map)
            m_properties.pop_back();
    }
    m_arrayTypes.pop_back();
}

void DesignViewBuilder::addProperty(
    const std::string& name,
    const std::string& typeName,
    const std::string& initialValue)
{
    auto layout = createPropertyLayout();
    //layout->setName(name);
    layout->addObject(createLabel(name));
    auto textEdit = createTextEdit();
    textEdit->setName("value");
    textEdit->setText(initialValue);
    layout->addObject(textEdit);
    currentPropertiesForPrimitive(typeName).layout->addObject(layout);
}

DesignViewBuilder::Properties DesignViewBuilder::createPropertiesImpl(
    int parentID, const std::string& buttonText)
{
    auto button = createObjectButton(buttonText);
    Properties props;
    props.id = m_treeView.addObject(parentID, button);
    props.layout = createPropertiesListLayout();
    m_propertiesMenu.addObject(props.id, props.layout);
    button->setCallback(std::bind(&ObjectsSelector::select, &m_propertiesMenu, props.id));
    return props;
}

DesignViewBuilder::Properties DesignViewBuilder::createProperties(
    const std::string& name, const std::string& typeName)
{
    int parentID = m_properties.back().id;
    std::string buttonText;
    ObjType::Enum parentObj = parentObjType();
    if (parentObj == ObjType::Array) {
        buttonText = mergeStrings("element", typeName);
    } else if (parentObj == ObjType::Map) {
        if (m_arrayTypes.back() == SerializationTag::Keys) {
            auto elementProperties = createPropertiesImpl(
                m_properties.back().id, "element");
            parentID = elementProperties.id;
            buttonText = mergeStrings("key", typeName);
            m_mapProperties.back().elements.push_back(elementProperties);
        } else {
            auto& mapProperties = m_mapProperties.back();
            auto elementProperties = mapProperties.elements.at(mapProperties.currentElem++);
            parentID = elementProperties.id;
            buttonText = mergeStrings("value", typeName);
        }
    } else {
        buttonText = mergeStrings(name, typeName);
    }

    return createPropertiesImpl(parentID, buttonText);
}

std::string DesignViewBuilder::propertyName(const std::string& nameFromSerializer)
{
    if (nameFromSerializer.empty()) {
        ObjType::Enum parentObj = parentObjType();
        if (parentObj == ObjType::Array)
            return "element";
        if (parentObj == ObjType::Map) {
            SerializationTag::Type arrayType = m_arrayTypes.back();
            if (arrayType == ObjType::PrimitiveArray)
                arrayType = *(m_arrayTypes.rbegin() + 1);
            if (arrayType == SerializationTag::Keys)
                return "key";
            if (arrayType == SerializationTag::Values)
                return "value";
            THROW_EX() << "Bad map's serialialization tag: " << arrayType;
        }
        return m_curName;
    }
    return nameFromSerializer;
}

DesignViewBuilder::Properties DesignViewBuilder::currentPropertiesForPrimitive(
    const std::string& typeName)
{
    if (m_objTypes.back() == ObjType::Array || m_objTypes.back() == ObjType::Map)
        return createProperties("", typeName);
    return m_properties.back();
}

DesignViewBuilder::ObjType::Enum DesignViewBuilder::parentObjType() const
{
    ObjType::Enum parentObj = ObjType::Unknown;
    for (auto it = m_objTypes.rbegin(); it != m_objTypes.rend(); ++it)
        if (*it != ObjType::Unknown && *it != ObjType::PrimitiveArray) {
            parentObj = *it;
            break;
        }
    return parentObj;
}

DesignViewBuilder::Properties DesignViewBuilder::currentProperties()
{
    return m_properties.back();
}

} }
