/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/ISerializable.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdint.h>

namespace gamebase { namespace impl {

class FontMetaData : public ISerializable {
public:
    FontMetaData();

    FontMetaData(int firstGlyph, int lastGlyph);

    std::vector<uint32_t> glyphIndices(const std::string& utf8Str) const;
        
    virtual void serialize(Serializer& s) const override;

private:
    uint32_t getGlyphIndex(uint64_t utf8CharCode) const;

    friend std::unique_ptr<IObject> deserializeFontMetaData(Deserializer&);

    std::unordered_map<uint64_t, uint32_t> m_glyphIndices;
};

} }
