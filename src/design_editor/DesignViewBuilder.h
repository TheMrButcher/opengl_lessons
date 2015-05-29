#pragma once

#include "TreeView.h"
#include "DesignModel.h"
#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

class TextEdit;
    
namespace editor {

class DesignViewBuilder : public ISerializer {
public:
    DesignViewBuilder(
        TreeView& treeView,
        ObjectsSelector& propertiesMenu,
        DesignModel& model,
        int rootID = TreeView::ROOT_ID);

    virtual void writeFloat(const std::string& name, float f) override;
    virtual void writeDouble(const std::string& name, double d) override;
    virtual void writeInt(const std::string& name, int i) override;
    virtual void writeUInt(const std::string& name, unsigned int i) override;
    virtual void writeInt64(const std::string& name, int64_t i) override;
    virtual void writeUInt64(const std::string& name, uint64_t i) override;
    virtual void writeBool(const std::string& name, bool b) override;
    virtual void writeString(const std::string& name, const std::string& value) override;
    virtual void startObject(const std::string& name) override;
    virtual void finishObject() override;
    virtual void startArray(const std::string& name, SerializationTag::Type tag) override;
    virtual void finishArray() override;

private:
    void addProperty(
        const std::string& name,
        const std::string& typeName,
        const std::string& initialValue,
        const std::function<void(TextEdit*, std::string, Json::Value*)>& updater);

    struct ObjType {
        enum Enum {
            Unknown,
            PrimitiveArray,
            Object,
            Array,
            Map
        };
    };

    struct Properties {
        int id;
        std::shared_ptr<LinearLayout> layout;
    };
    
    Properties createPropertiesImpl(int parentID, const std::string& buttonText);
    Properties createProperties(const std::string& name, const std::string& typeName);
    Properties currentPropertiesForPrimitive(const std::string& typeName);
    ObjType::Enum parentObjType() const;

    std::string propertyName(const std::string& nameFromSerializer);

    Properties currentProperties();

    TreeView& m_treeView;
    ObjectsSelector& m_propertiesMenu;
    DesignModel& m_model;
    std::vector<ObjType::Enum> m_objTypes;
    std::vector<SerializationTag::Type> m_arrayTypes;
    std::vector<Properties> m_properties;
    std::string m_curName;
    size_t m_primitiveElementIndex;
    int m_curModelNodeID;

    struct MapProperties {
        MapProperties() : currentElem(0) {}

        std::vector<Properties> elements;
        size_t currentElem;
    };

    std::vector<MapProperties> m_mapProperties;
};

} }