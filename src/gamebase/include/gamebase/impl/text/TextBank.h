/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <unordered_map>
#include <string>

namespace gamebase { namespace impl {

class GAMEBASE_API TextBank : public ISerializable {
public:
    void insert(const std::string& key, const std::string& value)
    {
        m_db[key] = value;
    }

    template <typename MapType>
    void insert(const MapType& newValues)
    {
        m_db.insert(newValues.begin(), newValues.end());
    }

    std::string get(const std::string& key) const
    {
        auto it = m_db.find(key);
        if (it == m_db.end())
            return key;
        return it->second;
    }

    std::string get(const std::string& key, int index) const;

    void add(const TextBank& other);

    virtual void serialize(Serializer& s) const override;

private:
    std::unordered_map<std::string, std::string> m_db;
};

GAMEBASE_API void loadTextBank(const std::string& path);
GAMEBASE_API TextBank* textBank();

} }
