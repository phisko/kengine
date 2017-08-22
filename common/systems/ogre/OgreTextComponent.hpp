#pragma once

#include "Component.hpp"
#include "pogre/MovableText.hpp"

class OgreTextComponent : public kengine::Component<OgreTextComponent>
{
public:
    // OgreTextComponent(std::string_view text, std::size_t textSize, std::string_view font)
    OgreTextComponent(Ogre::MovableText &text, Ogre::SceneNode &node)
            // : _text(text, textSize, font)
            : _text(text), _node(node)
    {}

public:
    // void setText(std::string_view text) { _text.setText(text); }
    // void setPosition(const putils::Point3d &pos) { _text.setPos({ pos.x, pos.y }); }
    void setText(std::string_view text) { _text.setCaption(text.data()); }
    void setPosition(const putils::Point3d &pos) { _node.setPosition((float)pos.x, (float)pos.y, (float)pos.z); }

public:
    Ogre::MovableText &getText() { return _text; }
    const Ogre::MovableText &getText() const { return _text; }

public:
    Ogre::SceneNode &getNode() { return _node; }
    const Ogre::SceneNode &getNode() const { return _node; }

public:
    std::string toString() const noexcept final { return "{type:ogretext}"; }

private:
    // pogre::Text _text;
    Ogre::MovableText &_text;
    Ogre::SceneNode &_node;
};