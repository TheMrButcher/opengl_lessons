/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/GroupLayer.h>
#include <gamebase/impl/gameview/GameView.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

GroupLayer::GroupLayer()
    : m_gameView(nullptr)
    , m_inited(false)
{
    m_canvas = std::make_shared<CanvasLayout>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())); 
    m_canvas->setParentPosition(this);
}

void GroupLayer::setOrder(const std::shared_ptr<IOrder>& order)
{
    m_order = order;
}

int GroupLayer::addLayer(const std::shared_ptr<ILayer>& layer)
{
    if (!m_inited)
        THROW_EX() << "Can't add layer, GroupLayer is not already loaded";
    int id = m_gameView->nextID();
    insertLayer(id, layer);
    return id;
}

void GroupLayer::insertLayers(const std::map<int, std::shared_ptr<ILayer>>& layers)
{
    std::map<int, std::shared_ptr<IObject>> objects;
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        objects.insert(objects.end(),
            std::make_pair(it->first, std::shared_ptr<IObject>(it->second)));
        registerLayer(it->first, it->second.get());
    }
    m_canvas->insertObjects(objects);
}

void GroupLayer::setViewBox(const BoundingBox& viewBox)
{
    if (m_viewBox == viewBox)
        return;
    m_viewBox = viewBox;
    m_layersShift = ShiftTransform2(-m_viewBox.center());
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        it->second->setViewBox(viewBox);
}

void GroupLayer::setGameBox(const boost::optional<BoundingBox>& gameBox)
{
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        it->second->setGameBox(gameBox);
}

void GroupLayer::clear()
{
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        it->second->clear();
}

void GroupLayer::update()
{
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        it->second->update();
}

std::shared_ptr<IObject> GroupLayer::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    if (!m_order)
        THROW_EX() << "Order is not set for GroupLayer";

    auto transformedPoint = m_layersShift.inversed() * point;
    BoundingBox searchBox(transformedPoint);
    m_cachedFindables.clear();
    std::unordered_map<IFindable*, ILayer*> objToLayer; 
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it) {
        const auto& findables = it->second->findablesByBox(searchBox);
        m_cachedFindables.insert(m_cachedFindables.end(),
            findables.begin(), findables.end());
        for (auto it2 = findables.begin(); it2 != findables.end(); ++it2)
            objToLayer[*it2] = it->second;
    }
    m_order->sort(m_cachedFindables);
    for (auto it = m_cachedFindables.rbegin(); it != m_cachedFindables.rend(); ++it) {
        if (auto obj = (*it)->findChildByPoint(transformedPoint))
            return obj;
        if ((*it)->isSelectableByPoint(transformedPoint))
            return objToLayer[*it]->getIObjectSPtr(*it);
    }
    return nullptr;
}

void GroupLayer::loadResources()
{
    m_canvas->loadResources();
}

void GroupLayer::drawAt(const Transform2& position) const
{
    if (!m_order)
        THROW_EX() << "Order is not set for GroupLayer";
    m_cachedDrawables.clear();
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it) {
        const auto& drawables = it->second->drawablesInView();
        m_cachedDrawables.insert(m_cachedDrawables.end(),
            drawables.begin(), drawables.end());
    }
    m_order->sort(m_cachedDrawables);
    auto fullTransform = m_layersShift * position;
    for (auto it = m_cachedDrawables.begin(); it != m_cachedDrawables.end(); ++it)
        (*it)->draw(fullTransform);
}

void GroupLayer::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_canvas->objectsCollection());
    if (!m_inited) {
        if (!m_register.parent())
            return;
        m_gameView = m_register.findParentOfType<GameView>();
        m_inited = true;
        for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
            m_gameView->registerLayer(it->first, it->second);
    }
}
    
void GroupLayer::serialize(Serializer& s) const
{
    s << "layers" << m_canvas->objectsAsMap() << "order" << m_order;
}

std::unique_ptr<IObject> deserializeGroupLayer(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<ILayer>> Layers;
    DESERIALIZE(Layers, layers);
    DESERIALIZE(std::shared_ptr<IOrder>, order);
    std::unique_ptr<GroupLayer> result(new GroupLayer());
    result->setOrder(order);
    result->insertLayers(layers);
    return std::move(result);
}

REGISTER_CLASS(GroupLayer);

void GroupLayer::registerLayer(int id, ILayer* layer)
{
    m_layers[id] = layer;
    if (m_inited)
        m_gameView->registerLayer(id, layer);
}

void GroupLayer::removeFromRegister(int id)
{
    m_layers.erase(id);
    if (m_inited)
        m_gameView->removeFromRegister(id);
}

} }
