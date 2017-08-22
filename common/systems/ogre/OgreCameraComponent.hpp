#pragma once

class OgreCameraComponent : public kengine::Component<OgreCameraComponent>
{
public:
    OgreCameraComponent(pogre::CameraMan &cameraMan, pogre::FreeFloatingStrategy &strategy)
            : _cameraMan(cameraMan), _strategy(strategy)
    {}

public:
    Ogre::SceneNode &getNode() noexcept { return *_strategy.getCamNode(); }
    const Ogre::SceneNode &getNode() const noexcept { return *_strategy.getCamNode(); }

public:
    void setPosition(const putils::Point3d &pos)
    {
        _strategy.getCamNode()->setPosition((float)pos.x, (float)pos.y, (float)pos.z);
    }

public:
    std::string toString() const noexcept final { return "{type:ogrecamera}"; }

private:
    pogre::CameraMan &_cameraMan;
    pogre::FreeFloatingStrategy &_strategy;
};