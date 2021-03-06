/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/tools/Exception.h>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <iostream>
#include <memory>

namespace gamebase { namespace impl {
namespace internal {
inline void defaultSerialize(const IObject* obj, Serializer& serializer)
{
    if (const auto* serObj = dynamic_cast<const ISerializable*>(obj)) {
        serObj->serialize(serializer);
    } else {
        THROW_EX() << "Type (type_index: " << typeid(*obj).name() << ") is not ISerializable";
    }
}
}
class Deserializer;

class GAMEBASE_API SerializableRegister {
public:
    static SerializableRegister& instance()
    {
        static SerializableRegister reg;
        return reg;
    }

    template <typename T>
    void registerType(
        const std::string& typeName,
        const std::function<std::unique_ptr<IObject>(Deserializer&)>& deserialize)
    {
        registerType<T>(typeName, deserialize, &internal::defaultSerialize);
    }

    template <typename T>
    void registerType(
        const std::string& typeName,
        const std::function<std::unique_ptr<IObject>(Deserializer&)>& deserialize,
        const std::function<void(const IObject*, Serializer&)>& serialize)
    {
        std::type_index typeIndex = typeid(T);
        TypeTraits traits(typeName, typeIndex, deserialize, serialize);
        {
            auto itAndFlag = m_nameToType.insert(std::make_pair(typeName, traits));
            if (!itAndFlag.second)
                itAndFlag.first->second = traits;
        }
        {
            auto itAndFlag = m_indexToType.insert(std::make_pair(typeIndex, traits));
            if (!itAndFlag.second)
                itAndFlag.first->second = traits;
        }
    }

    struct TypeTraits {
        TypeTraits(
            const std::string& typeName,
            const std::type_index& index,
            const std::function<std::unique_ptr<IObject>(Deserializer&)>& deserialize,
            const std::function<void(const IObject*, Serializer&)>& serialize)
            : typeName(typeName)
            , index(index)
            , serialize(serialize)
            , deserialize(deserialize)
        {}

        std::string typeName;
        std::type_index index;
        std::function<std::unique_ptr<IObject>(Deserializer&)> deserialize;
        std::function<void(const IObject*, Serializer&)> serialize;
    };

    bool isRegistered(const std::string& name) const
    {
        return m_nameToType.count(name) > 0;
    }

    template <typename T>
    bool isRegistered() const
    {
        return isRegistered(typeid(T));
    }

    bool isRegistered(const std::type_info& typeInfo) const
    {
        return isRegistered(std::type_index(typeInfo));
    }

    bool isRegistered(const std::type_index& typeIndex) const
    {
        return m_indexToType.count(typeIndex) > 0;
    }

    const TypeTraits& typeTraits(const std::string& name) const
    {
        auto it = m_nameToType.find(name);
        if (it == m_nameToType.end())
            THROW_EX() << "Type " << name << " is not registered";
        return it->second;
    }

    template <typename T>
    const std::string& typeName() const
    {
        return typeName(typeid(T));
    }

    const std::string& typeName(const std::type_info& typeInfo) const
    {
        return typeName(std::type_index(typeInfo));
    }
    
    const std::string& typeName(const std::type_index& typeIndex) const
    {
        return typeTraits(typeIndex).typeName;
    }

    template <typename T>
    const TypeTraits& typeTraits() const
    {
        return typeTraits(typeid(T));
    }

    const TypeTraits& typeTraits(const std::type_info& typeInfo) const
    {
        return typeTraits(std::type_index(typeInfo));
    }

    const TypeTraits& typeTraits(const std::type_index& typeIndex) const
    {
        auto it = m_indexToType.find(typeIndex);
        if (it == m_indexToType.end())
            THROW_EX() << "Type with index " << typeIndex.name() << " is not registered.";
        return it->second;
    }

    void debugPrint() const
    {
        std::cout << "Register ID: " << reinterpret_cast<unsigned int>(this) << std::endl;
        std::cout << "Registered classes: " << m_nameToType.size() << std::endl;
        for (auto it = m_nameToType.begin(); it != m_nameToType.end(); ++it)
            std::cout << it->first << " (type_index: " << it->second.index.name() << ")" << std::endl;
    }

private:
    SerializableRegister() {}

    std::unordered_map<std::string, TypeTraits> m_nameToType;
    std::unordered_map<std::type_index, TypeTraits> m_indexToType;
};

#define REGISTER_TEMPLATE(className, argName) \
    namespace { \
    class _GamebaseRegister##className##argName { \
    public: \
        _GamebaseRegister##className##argName() \
        { \
            gamebase::impl::SerializableRegister::instance().registerType<className<argName>>( \
                #className"<"#argName">", &deserialize##className<argName>); \
        } \
    } _gamebaseRegister##className##argName; \
    }

#define REGISTER_CLASS2(className, deserializerName) \
    namespace { \
    class _GamebaseRegister##className { \
    public: \
        _GamebaseRegister##className() \
        { \
            gamebase::impl::SerializableRegister::instance().registerType<className>(#className, deserializerName); \
        } \
    } _gamebaseRegister##className; \
    }

#define REGISTER_CLASS(className) REGISTER_CLASS2(className, deserialize##className)

} }
