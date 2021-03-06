/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/SerializableRegister.h>
#include <gamebase/impl/serial/constants.h>
#include <gamebase/impl/engine/RelativeValue.h>
#include <gamebase/impl/app/TimeState.h>
#include <gamebase/impl/reg/IRegistrable.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/text/AlignProperties.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/impl/graphics/GLColor.h>
#include <boost/optional.hpp>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace gamebase { namespace impl {

class IDeserializer {
public:
    virtual ~IDeserializer() {}

    virtual SerializationVersion version() const = 0;

    virtual bool hasMember(const std::string& name) = 0;

    virtual float readFloat(const std::string& name) = 0;

    virtual double readDouble(const std::string& name) = 0;

    virtual int readInt(const std::string& name) = 0;

    virtual unsigned int readUInt(const std::string& name) = 0;

    virtual int64_t readInt64(const std::string& name) = 0;

    virtual uint64_t readUInt64(const std::string& name) = 0;

    virtual bool readBool(const std::string& name) = 0;

    virtual std::string readString(const std::string& name) = 0;
    
    virtual void startObject(const std::string& name) = 0;

    virtual void finishObject() = 0;

    virtual void startArray(const std::string& name, SerializationTag::Type tag) = 0;

    virtual size_t arraySize(const std::string& name) = 0;

    virtual void finishArray() = 0;
};

class Deserializer {
public:
    Deserializer(IDeserializer* deserializer)
        : m_deserializer(deserializer)
    {}

    class ValueDeserializer {
    public:
        ValueDeserializer(IDeserializer* deserializer, const std::string& name)
            : m_deserializer(deserializer)
            , m_name(name)
        {}

        Deserializer operator>>(float& f) const
        {
            f = m_deserializer->readFloat(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(double& d) const
        {
            d = m_deserializer->readDouble(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(int& i) const
        {
            i = m_deserializer->readInt(m_name);
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(unsigned int& i) const
        {
            i = m_deserializer->readUInt(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(int64_t& i) const
        {
            i = m_deserializer->readInt64(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(uint64_t& i) const
        {
            i = m_deserializer->readUInt64(m_name);
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(bool& b) const
        {
            b = m_deserializer->readBool(m_name);
            return Deserializer(m_deserializer);
        }

        template <typename EnumType>
        typename std::enable_if<std::is_enum<EnumType>::value, Deserializer>::type operator>>(
            EnumType& enumValue) const
        {
            enumValue = static_cast<EnumType>(m_deserializer->readInt(m_name));
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(std::string& s) const
        {
            s = m_deserializer->readString(m_name);
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Vec2& v) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Vec2);
            readVector(v);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(Matrix2& m) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Matrix2);
            readMatrix(m);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(Transform2& t) const
        {
            m_deserializer->startArray(m_name, SerializationTag::Transform2);
            readMatrix(t.matrix);
            readVector(t.offset);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(BoundingBox& b) const
        {
            m_deserializer->startArray(m_name, SerializationTag::BoundingBox);
            readVector(b.bottomLeft);
            readVector(b.topRight);
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(GLColor& c) const
        {
            m_deserializer->startArray(m_name, SerializationTag::GLColor);
            c.r = m_deserializer->readFloat("");
            c.g = m_deserializer->readFloat("");
            c.b = m_deserializer->readFloat("");
            c.a = m_deserializer->readFloat("");
            m_deserializer->finishArray();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(RelativeValue& relVal) const
        {
            m_deserializer->startObject(m_name);
            Deserializer relValDeserializer(m_deserializer);
            RelType::Enum type;
            relValDeserializer >> "type" >> type;
            if (type == RelType::Identic) {
                relVal = RelativeValue();
            } else {
                float value;
                relValDeserializer >> "value" >> value;
                relVal = RelativeValue(type, value);
            }
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(TypedTime& time) const
        {
            m_deserializer->startObject(m_name);
            Deserializer timeDeserializer(m_deserializer);
            timeDeserializer >> "type" >> time.type >> "value" >> time.value;
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }
        
        Deserializer operator>>(FontDesc& fontDesc) const
        {
            m_deserializer->startObject(m_name);
            Deserializer fontDescDeserializer(m_deserializer);
            std::string family;
            float size;
            fontDescDeserializer >> "family" >> family >> "size" >> size;
            if (!family.empty())
                fontDesc.fontFamily = family;
            if (size > 0)
                fontDesc.size = size;
            if (fontDescDeserializer.hasMember("bold"))
                fontDescDeserializer >> "bold" >> fontDesc.bold;
            if (fontDescDeserializer.hasMember("italic"))
                fontDescDeserializer >> "italic" >> fontDesc.italic;
            if (fontDescDeserializer.hasMember("underlined"))
                fontDescDeserializer >> "underlined" >> fontDesc.underlined;
            if (fontDescDeserializer.hasMember("lineThrough"))
                fontDescDeserializer >> "lineThrough" >> fontDesc.lineThrough;
            if (fontDescDeserializer.hasMember("outlineWidth")) {
                float outlineWidth;
                fontDescDeserializer >> "outlineWidth" >> outlineWidth;
                if (outlineWidth > 0)
                    fontDesc.outlineWidth = outlineWidth;
            }
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        Deserializer operator>>(AlignProperties& props) const
        {
            m_deserializer->startObject(m_name);
            Deserializer propsDeserializer(m_deserializer);
            propsDeserializer >> "font" >> props.font >> "horAlign" >> props.horAlign
                >> "vertAlign" >> props.vertAlign >> "enableStacking" >> props.enableStacking;
            m_deserializer->finishObject();
            return Deserializer(m_deserializer);
        }

        template <typename T>
        typename std::enable_if<std::is_base_of<IObject, T>::value, Deserializer>::type operator>>(T& obj) const
        {
            return deserializeObject<T>(obj);
        }
        
        template <typename T>
        typename std::enable_if<std::is_base_of<IObject, T>::value, Deserializer>::type operator>>(std::shared_ptr<T>& obj) const
        {
            return deserializeObject<T>(obj);
        }

        template <typename T>
        Deserializer operator>>(std::vector<T>& collection) const
        {
            try {
                if (m_deserializer->version() == SerializationVersion::VER2)
                    deserializeArrayLegacy(collection);
                else
                    deserializeArray3(collection);
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize collection " << m_name << ". Reason: " << ex.what();
            }
            return Deserializer(m_deserializer);
        }

        template <typename K, typename V>
        Deserializer operator>>(std::map<K, V>& m) const
        {
            return deserializeMap<K, V>(m);
        }
        
        template <typename K, typename V>
        Deserializer operator>>(std::unordered_map<K, V>& m) const
        {
            return deserializeMap<K, V>(m);
        }

    private:
        template <typename T>
        void deserializeArrayLegacy(std::vector<T>& collection) const
        {
            m_deserializer->startObject(m_name);
            size_t size = static_cast<size_t>(m_deserializer->readInt(COLLECTION_SIZE_TAG));
            readArray(COLLECTION_ELEMENTS_TAG, collection, size, SerializationTag::Array);
            m_deserializer->finishObject();
        }

        template <typename T>
        void deserializeArray3(std::vector<T>& collection) const
        {
            size_t size = m_deserializer->arraySize(m_name);
            readArray(m_name, collection, size, SerializationTag::Array);
        }

        void readMatrix(Matrix2& m) const
        {
            float m00 = m_deserializer->readFloat("");
            float m01 = m_deserializer->readFloat("");
            float m10 = m_deserializer->readFloat("");
            float m11 = m_deserializer->readFloat("");
            m = Matrix2(m00, m01, m10, m11);
        }

        void readVector(Vec2& v) const
        {
            v.x = m_deserializer->readFloat("");
            v.y = m_deserializer->readFloat("");
        }

        template <typename T>
        void readArray(
            const std::string& name, std::vector<T>& collection,
            size_t size, SerializationTag::Type tag) const
        {
            collection.clear();
            collection.reserve(size);
            m_deserializer->startArray(name, tag);
            ValueDeserializer elemsDeserializer(m_deserializer, "");
            for (size_t i = 0; i < size; ++i) {
                T elem;
                elemsDeserializer >> elem;
                collection.push_back(std::move(elem));
            }
            m_deserializer->finishArray();
        }
        
        template <typename K, typename V, typename Map>
        Deserializer deserializeMap(Map& m) const
        {
            try {
                if (m_deserializer->version() == SerializationVersion::VER2)
                    deserializeMapLegacy<K, V>(m);
                else
                    deserializeMap3<K, V>(m);
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize map " << m_name << ". Reason: " << ex.what();
            }
            return Deserializer(m_deserializer);
        }

        template <typename K, typename V, typename Map>
        void deserializeMapLegacy(Map& m) const
        {
            m_deserializer->startObject(m_name);
            size_t size = static_cast<size_t>(m_deserializer->readInt(COLLECTION_SIZE_TAG));
            std::vector<K> keys;
            readArray(MAP_KEYS_TAG, keys, size, SerializationTag::Keys);
            std::vector<V> values;
            readArray(MAP_VALUES_TAG, values, size, SerializationTag::Values);
            m.clear();
            for (size_t i = 0; i < keys.size(); ++i)
                m[std::move(keys[i])] = std::move(values[i]);
            m_deserializer->finishObject();
        }

        template <typename K, typename V, typename Map>
        void deserializeMap3(Map& m) const
        {
            size_t size = m_deserializer->arraySize(m_name);
            m_deserializer->startArray(m_name, SerializationTag::Map);
            m.clear();
            for (size_t i = 0; i < size; ++i) {
                m_deserializer->startObject("");

                K key;
                ValueDeserializer keyDeserializer(m_deserializer, MAP_KEY_TAG);
                keyDeserializer >> key;

                V value;
                ValueDeserializer valueDeserializer(m_deserializer, MAP_VALUE_TAG);
                valueDeserializer >> value;

                m[std::move(key)] = std::move(value);
                m_deserializer->finishObject();
            }
            m_deserializer->finishArray();
        }

        template <typename T, typename U>
        Deserializer deserializeObject(U& obj) const
        {
            std::string typeName("unknown");
            try {
                if (m_deserializer->version() == SerializationVersion::VER2)
                    deserializeObjectLegacy<T>(obj, typeName);
                else
                    deserializeObject3<T>(obj, typeName);
            } catch (const std::exception& ex) {
                THROW_EX() << "Can't deserialize object " << m_name << " (type: " << typeName << "). Reason: " << ex.what();
            }
            return Deserializer(m_deserializer);
        }

        template <typename T, typename U>
        void deserializeObjectLegacy(U& obj, std::string& typeName) const
        {
            boost::optional<SerializableRegister::TypeTraits> traits;
            m_deserializer->startObject(m_name);
            std::unique_ptr<IObject> rawObj;
            bool isEmpty = m_deserializer->readBool(EMPTY_TAG);
            Deserializer objectDeserializer(m_deserializer);
            if (!isEmpty) {
                typeName = m_deserializer->readString(TYPE_NAME_TAG);
                traits = SerializableRegister::instance().typeTraits(typeName);
                rawObj = traits->deserialize(objectDeserializer);
            }
            if (rawObj) {
                if (IRegistrable* regObj = dynamic_cast<IRegistrable*>(rawObj.get())) {
                    std::string objName;
                    objectDeserializer >> REG_NAME_TAG >> objName;
                    regObj->setName(objName);
                }
                if (IDrawable* drawObj = dynamic_cast<IDrawable*>(rawObj.get())) {
                    bool visible;
                    objectDeserializer >> VISIBLE_TAG >> visible;
                    drawObj->setVisible(visible);
                }
                if (T* cnvObj = dynamic_cast<T*>(rawObj.get())) {
                    rawObj.release();
                    std::unique_ptr<T> cnvObjUPtr(cnvObj);
                    setObject(cnvObjUPtr, obj);
                } else {
                    THROW_EX() << "Type " << typeName << " (type_index: " << traits->index.name()
                        << ") is not convertible to type " << typeid(T).name();
                }
            } else {
                resetObject(obj);
            }
            m_deserializer->finishObject();
        }

        template <typename T, typename U>
        void deserializeObject3(U& obj, std::string& typeName) const
        {
            boost::optional<SerializableRegister::TypeTraits> traits;
            m_deserializer->startObject(m_name);
            std::unique_ptr<IObject> rawObj;
            bool isEmpty = false;
            if (m_deserializer->hasMember(EMPTY_TAG))
                isEmpty = m_deserializer->readBool(EMPTY_TAG);
            Deserializer objectDeserializer(m_deserializer);
            if (!isEmpty) {
                typeName = m_deserializer->readString(TYPE_NAME_TAG);
                traits = SerializableRegister::instance().typeTraits(typeName);
                rawObj = traits->deserialize(objectDeserializer);
            }
            if (rawObj) {
                if (objectDeserializer.hasMember(REG_NAME_TAG)) {
                    if (IRegistrable* regObj = dynamic_cast<IRegistrable*>(rawObj.get())) {
                        std::string objName;
                        objectDeserializer >> REG_NAME_TAG >> objName;
                        regObj->setName(objName);
                    }
                }
                if (objectDeserializer.hasMember(VISIBLE_TAG)) {
                    if (IDrawable* drawObj = dynamic_cast<IDrawable*>(rawObj.get())) {
                        bool visible;
                        objectDeserializer >> VISIBLE_TAG >> visible;
                        drawObj->setVisible(visible);
                    }
                }
                if (T* cnvObj = dynamic_cast<T*>(rawObj.get())) {
                    rawObj.release();
                    std::unique_ptr<T> cnvObjUPtr(cnvObj);
                    setObject(cnvObjUPtr, obj);
                } else {
                    THROW_EX() << "Type " << typeName << " (type_index: " << traits->index.name()
                        << ") is not convertible to type " << typeid(T).name();
                }
            } else {
                resetObject(obj);
            }
            m_deserializer->finishObject();
        }

        template <typename T>
        void setObject(std::unique_ptr<T>& cnvObj, T& obj) const
        {
            obj = std::move(*cnvObj);
        }
        
        template <typename T>
        void setObject(std::unique_ptr<T>& cnvObj, std::shared_ptr<T>& obj) const
        {
            obj.reset(cnvObj.release());
        }

        template <typename T>
        void resetObject(T& obj) const
        {
            THROW_EX() << "Can't reset reference on object (marked as empty)";
        }
        
        template <typename T>
        void resetObject(std::shared_ptr<T>& obj) const
        {
            obj.reset();
        }

        IDeserializer* m_deserializer;
        std::string m_name;
    };

    ValueDeserializer operator>>(const std::string& name) const
    {
        return ValueDeserializer(m_deserializer, name);
    }

    bool hasMember(const std::string& name) const
    {
        return m_deserializer->hasMember(name);
    }

private:
    IDeserializer* m_deserializer;
};

#define DESERIALIZE(Type, value) \
    Type value; \
    deserializer >> #value >> value

#define DESERIALIZE_OPT(Type, value, defaultValue) \
    Type value = defaultValue; \
    if (deserializer.hasMember(#value)) \
        deserializer >> #value >> value

} }
