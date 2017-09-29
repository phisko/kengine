#pragma once

class OgreLightComponent : public kengine::Component<OgreLightComponent>
{
public:
    OgreLightComponent(Ogre::Light &light)
            : _light(light)
    {}

public:
    Ogre::Light &getLight() noexcept { return _light; }
    const Ogre::Light &getLight() const noexcept { return _light; }

public:
    void setPosition(const putils::Point3d &pos)
    {
        _light.setPosition({ (float)pos.x, (float)pos.y, (float)pos.z });
    }

public:
    std::string toString() const noexcept final { return "{type:ogrelight}"; }

private:
    Ogre::Light &_light;
};