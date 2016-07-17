/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/Layer.h>
#include "LayerHelpers.h"
#include <gamebase/impl/serial/IDeserializer.h>
#include "src/impl/global/GlobalTemporary.h"

namespace gamebase { namespace impl {

Layer::Layer()
    : ImmobileLayer()
{}
    
void Layer::drawAt(const Transform2& position) const
{
    ImmobileLayer::drawAt(position);
    g_temp.delayedTasks.push_back(std::bind(
        &Layer::setNeedUpdate, this));
}

std::unique_ptr<IObject> deserializeLayer(Deserializer& deserializer)
{
    std::unique_ptr<Layer> result(new Layer());
    deserializeLayerContents(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(Layer);

const std::vector<Drawable*>& Layer::drawablesInView() const
{
    g_temp.delayedTasks.push_back(std::bind(
        &Layer::setNeedUpdate, this));
    return ImmobileLayer::drawablesInView();
}

} }