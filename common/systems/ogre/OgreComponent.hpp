#pragma once

#include <Component.hpp>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

class OgreComponent : public kengine::Component<OgreComponent>
{
public:
    OgreComponent(Ogre::Entity &entity, Ogre::SceneNode &node, pogre::App &app)
            : _entity(entity),
              _node(node),
              _originalSize(entity.getBoundingBox().getSize()),
              _app(app)
    {}

public:
    Ogre::SceneNode &getNode() { return _node; }
    const Ogre::SceneNode &getNode() const { return _node; }

public:
    Ogre::Entity &getEntity() { return _entity; }
    const Ogre::Entity &getEntity() const { return _entity; }

public:
    void setWidth(double width) { setScale(width, &Ogre::Vector3::x); }
    void setHeight(double height) { setScale(height, &Ogre::Vector3::y); }
    void setLength(double length) { setScale(length, &Ogre::Vector3::z); }

private:
    template<typename Ptr>
    void setScale(double length, Ptr member)
    {
        const auto scale = length / _originalSize.*member;
        _node.setScale(float(scale), float(scale), float(scale));
    }

public:
    void setPos(const putils::Point<double, 3> &pos)
    {
        const auto heightMod = 0 - _entity.getBoundingBox().getMinimum().y * _node.getScale().y;
        _node.setPosition({ float(pos.x), float(heightMod + pos.y), float(pos.z) });
    }

public:
    void setOrientation(double pitchRadians, double yawRadians)
    {
        _node.resetOrientation();
        _node.pitch(Ogre::Radian((float)pitchRadians));
        _node.yaw(Ogre::Radian((float)yawRadians));
    }

public:
    std::string toString() const final { return "{type:ogre}"; }

private:
    Ogre::Entity &_entity;
    Ogre::SceneNode &_node;

private:
    Ogre::Vector3 _originalSize;
    pogre::App &_app;
};
